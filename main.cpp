#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <clang/AST/AST.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/ASTConsumers.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <nlohmann/json.hpp>
#include <filesystem>

#include "ExtractTools.h"
#include "CppParser.h"

using namespace clang;
using namespace clang::tooling;
using json = nlohmann::json;

std::string file_patch = std::filesystem::current_path().string();                 ///< 只解析这个路径下的文件
/**
 * @brief 系统列表
 * @tparam std::string 系统名
 * @tparam CPPPARSER::system 系统
 */
static std::map<std::string, CPPPARSER::system> systemList;

class ClassVisitor : public RecursiveASTVisitor<ClassVisitor>
{
public:
    explicit ClassVisitor(ASTContext *Context) : _context(Context)
    {}

    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration)
    {
        FullSourceLoc FullLocation = _context->getFullLoc(Declaration->getBeginLoc());
        if (FullLocation.isValid())
        {
            const SourceManager &SM = _context->getSourceManager();
            StringRef FilePath = SM.getFilename(FullLocation);
            if (FilePath.find(file_patch.c_str()) == std::string::npos)
                return true;

            std::cout << "类: " << Declaration->getNameAsString();

            if (Declaration->isCompleteDefinition())
                std::cout << " 具有完整定义" << std::endl;
            else
                std::cout << " 不完全定义" << std::endl;

            if (const RawComment *Comment = _context->getRawCommentForDeclNoCache(Declaration))
            {
                std::string commentText = Comment->getRawText(_context->getSourceManager()).str();

                /// 1.抽取 @system 注释的内容
                std::string system_content = ExtractTools::ExtractSystemContent(commentText);
                /// 找到所属系统
                auto current_system = systemList.find(system_content);
                if(current_system == systemList.end())
                {
                    std::cout << "--增加系统：" << system_content << std::endl;
                    auto it = systemList.insert(std::make_pair(system_content, CPPPARSER::system(system_content)));
                    current_system = it.first;
                }

                /// 2.抽取 @module 注释内容
                std::string moduleContent = ExtractTools::ExtractModuleContent(commentText);
                /// 找到模块
                auto current_module = current_system->second._module_list.find(moduleContent);
                if(current_module == current_system->second._module_list.end())
                {
                    std::cout << "----增加模块：" << moduleContent << std::endl;
                    auto it = current_system->second._module_list.insert(std::make_pair(moduleContent, CPPPARSER::module(moduleContent)));
                    current_module = it.first;
                }

                /// 3.抽取 @sub_module 注释内容
                std::string sub_module_content = ExtractTools::ExtractSubModuleContent(commentText);
                if(!sub_module_content.empty())
                {
                    /// 找到子模块
                    auto current_sub_module = current_module->second._sub_module_list.find(sub_module_content);
                    if(current_sub_module == current_module->second._sub_module_list.end())
                    {
                        std::cout << "------增加子模块：" << sub_module_content << std::endl;
                        auto it = current_module->second._sub_module_list.insert(std::make_pair(sub_module_content, CPPPARSER::module(sub_module_content)));
                        current_module = it.first;
                    }
                }

                /// 抽取 @name 注释内容
                std::string class_name_content = ExtractTools::ExtractNameContent(commentText);
                /// 抽取 @brief 的内容
                std::string class_brief_content = ExtractTools::ExtractBriefContent(commentText);
                /// 4.找到类
                auto current_class = current_module->second._class_list.find(Declaration->getNameAsString());
                if(current_class == current_module->second._class_list.end())
                {
                    std::cout << "--------增加类：" << Declaration->getNameAsString() << std::endl;
                    auto it = current_module->second._class_list.insert(std::make_pair(Declaration->getNameAsString(), CPPPARSER::_class(
                            moduleContent + sub_module_content, Declaration->getNameAsString(),
                            system_content, moduleContent, sub_module_content, class_name_content,
                            class_brief_content)));
                    current_class = it.first;
                }

                /// 5.找到所有成员变量
                for (auto it = Declaration->field_begin(); it != Declaration->field_end(); ++it)
                {
                    FieldDecl *field = *it;
                    std::cout << "----------增加成员变量: " << field->getNameAsString() << std::endl;

                    /// 获取并打印成员变量的注释
                    std::string commentText;
                    if (const RawComment *Comment = _context->getRawCommentForDeclNoCache(field))
                        commentText = Comment->getRawText(_context->getSourceManager()).str();
                    /// 提取 @name
                    std::string current_name = ExtractTools::ExtractNameContent(commentText);
                    /// 提取 @brief
                    std::string current_brief = ExtractTools::ExtractBriefContent(commentText);
                    auto current_field = current_class->second._field_list.find(field->getNameAsString());
                    if(current_field == current_class->second._field_list.end())
                    {
                        current_class->second._field_list.insert(std::make_pair(field->getNameAsString(),
                                                                                CPPPARSER::_field(Declaration->getNameAsString(),
                                                                                                  field->getType().getAsString(),
                                                                                                  field->getNameAsString(), field->getAccess(),
                                                                                                  current_name, current_brief)));
                    }
                }

                /// 6.获取类的所有方法
                for (auto it = Declaration->method_begin(); it != Declaration->method_end(); ++it)
                {
                    current_class->second._has_method = true;
                    CXXMethodDecl *method = *it;
                    std::cout << "----------增加成员函数: " << method->getNameAsString() << std::endl;

                    /// 获取方法的注释
                    std::string current_func_comment;
                    if (const RawComment *Comment = _context->getRawCommentForDeclNoCache(method))
                        current_func_comment = Comment->getRawText(_context->getSourceManager()).str();

                    /// 抽取 @return 部分的注释
                    std::string current_return = ExtractTools::ExtractReturnContent(current_func_comment);
                    /// 抽取 @name 部分的注释
                    std::string current_func_name = ExtractTools::ExtractNameContent(current_func_comment);
                    /// 抽取 @brief 部分的注释
                    std::string current_func_brief = ExtractTools::ExtractBriefContent(current_func_comment);
                    auto current_func = current_class->second._method_list.find(method->getNameAsString());
                    if(current_func == current_class->second._method_list.end())
                    {
                        auto it_func = current_class->second._method_list.insert(std::make_pair(method->getNameAsString(),
                                                                                                CPPPARSER::_method(Declaration->getNameAsString(), method->getAccess(),
                                                                                                                   method->getNameAsString(), current_return,
                                                                                                                   current_func_name, current_func_brief)));
                        current_func = it_func.first;

                        /// 7.提取参数列表
                        for (const ParmVarDecl *param : method->parameters())
                        {
                            std::cout << "------------增加参数: " << param->getNameAsString() << std::endl;
                            current_func->second._param_list.insert(std::make_pair(
                                    param->getNameAsString(),
                                    CPPPARSER::_param(
                                            Declaration->getNameAsString(), method->getNameAsString(),
                                            param->getType().getAsString(), param->getNameAsString(),
                                            "", ""
                                    )
                            ));
                        }

                        /// 8.提取返回类型
                        std::cout << "------------增加返回类型: " << method->getDeclaredReturnType().getAsString() << std::endl;
                        current_func->second._return_type = CPPPARSER::returnType(
                                Declaration->getNameAsString(), method->getNameAsString(),
                                method->getDeclaredReturnType().getAsString(), current_return,
                                current_return
                        );
                    }

                    /// 9.提取参数 map
                    current_func->second._param_comment_map = ExtractTools::ExtractParamsContent(current_func_comment);
                    for(auto& iter : current_func->second._param_comment_map)
                    {
                        std::cout << "------------增加参数类型: " << std::endl;
                        std::cout << "-------------- " << iter.first << " : " << iter.second << std::endl;
                    }
//                    current_func->second._param_comment_map
                }
            }
            else
            {
                std::cout << "[WARNING]" << Declaration->getNameAsString() << "没有注释" << std::endl;
            }
            std::cout << std::endl;
        }
        return true;
    }

private:
    /// 类访问
    ASTContext* _context;
};

/// 消费 AST
class FunctionASTConsumer : public ASTConsumer
{
public:
    explicit FunctionASTConsumer(ASTContext *Context) : _visitor(Context)
    {}

    virtual void HandleTranslationUnit(ASTContext &Context)
    {
        _visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    ClassVisitor _visitor;
};

class FunctionFrontendAction : public ASTFrontendAction
{
public:
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file)
    {
        /// 抑制所有诊断信息
        CI.getDiagnostics().setSuppressAllDiagnostics(true);
        return std::make_unique<FunctionASTConsumer>(&CI.getASTContext());
    }
};

static llvm::cl::OptionCategory ToolingSampleCategory("tooling-sample");

void output_json_file(std::map<std::string, CPPPARSER::system>& systemList)
{
    json j;

    /// 获取系统或服务
    for(auto& system : systemList)
    {
        j["systemList"].push_back(system.second.ToJson());
    }

    std::ofstream outFile("output.json");
    outFile << std::setw(4) << j << std::endl;
    outFile.close();
}

void WriteCompileCommand()
{
    auto j = R"(
      [
        {
          "directory": ".",
          "command": "/usr/bin/g++ -frtti -fexceptions -g -std=gnu++17 -fdiagnostics-color=always   -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS",
          "file": "/a.cpp",
          "output": "/a.o"
        }
      ]
    )"_json;
    std::ofstream outFile("compile_commands.json");
    outFile << j << std::endl;
    outFile.close();
}

int main(int argc, const char **argv)
{
    /// 创建编译选项
    WriteCompileCommand();

    /// 创建一个 CommonOptionsParser 实例，用于解析命令行选项。
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolingSampleCategory);
    if (!ExpectedParser)
    {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser &OptionsParser = *ExpectedParser;

    std::vector<std::string> allFiles = OptionsParser.getSourcePathList();

    if(allFiles.size() == 1 && std::filesystem::is_directory(allFiles[0]))
    {
        std::string directory_path = allFiles[0];
        allFiles.clear();

        for(const auto& entry : std::filesystem::recursive_directory_iterator(directory_path))
        {
            if(entry.is_regular_file() && entry.path().extension() == ".h")
            {
                allFiles.push_back(entry.path().string());
                std::cout << "添加文件：" << entry.path().string() << std::endl;
            }
        }
        std::cout << std::endl;
    }

    /// 使用 CommonOptionsParser 创建一个 ClangTool 实例。
    ClangTool Tool(OptionsParser.getCompilations(), allFiles);

    /// 运行 ClangTool，并使用 FunctionFrontendAction 处理输入文件。
    int ret = Tool.run(newFrontendActionFactory<FunctionFrontendAction>().get());

    output_json_file(systemList);
    return 0;
}
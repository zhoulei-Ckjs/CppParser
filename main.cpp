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

#include "ExtractTools.h"
#include "CppParser.h"

using namespace clang;
using namespace clang::tooling;
using json = nlohmann::json;

const char * file_papth = "/home/zl/CppParser";                 ///< 只解析这个路径下的文件
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

    bool VisitFunctionDecl(FunctionDecl *Declaration)
    {
        FullSourceLoc FullLocation = _context->getFullLoc(Declaration->getBeginLoc());
        if (FullLocation.isValid())
        {
            const SourceManager &SM = _context->getSourceManager();
            StringRef FilePath = SM.getFilename(FullLocation);
            if (FilePath.find(file_papth) == std::string::npos)
                return true;

            std::cout << "函数: " << Declaration->getNameInfo().getName().getAsString();

            if (Declaration->hasBody())
                std::cout << " 有定义" << std::endl;
            else
                std::cout << " 没有完整定义" << std::endl;

            if (const RawComment *Comment = _context->getRawCommentForDeclNoCache(Declaration))
            {
                std::string commentText = Comment->getRawText(_context->getSourceManager()).str();
                std::cout << "\t" << commentText << std::endl;
            }
            else
            {
                std::cout << "\t没有注释" << std::endl;
            }
            std::cout << std::endl;
        }
        return true;
    }

    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration)
    {
        FullSourceLoc FullLocation = _context->getFullLoc(Declaration->getBeginLoc());
        if (FullLocation.isValid())
        {
            const SourceManager &SM = _context->getSourceManager();
            StringRef FilePath = SM.getFilename(FullLocation);
            if (FilePath.find(file_papth) == std::string::npos)
                return true;

            std::cout << "类: " << Declaration->getNameAsString();

            if (Declaration->isCompleteDefinition())
                std::cout << " 具有完整定义" << std::endl;
            else
                std::cout << " 不完全定义" << std::endl;

            if (const RawComment *Comment = _context->getRawCommentForDeclNoCache(Declaration))
            {
                std::string commentText = Comment->getRawText(_context->getSourceManager()).str();

                /// 抽取 @system 注释的内容
                std::string system_content = ExtractTools::ExtractSystemContent(commentText);
                /// 找到所属系统
                auto current_system = systemList.find(system_content);
                if(current_system == systemList.end())
                {
                    std::cout << "--增加系统：" << system_content << std::endl;
                    auto it = systemList.insert(std::make_pair(system_content, CPPPARSER::system(system_content)));
                    current_system = it.first;
                }

                /// 抽取 @module 注释内容
                std::string moduleContent = ExtractTools::ExtractModuleContent(commentText);
                /// 找到模块
                auto current_module = current_system->second._module_list.find(moduleContent);
                if(current_module == current_system->second._module_list.end())
                {
                    std::cout << "----增加模块：" << moduleContent << std::endl;
                    auto it = current_system->second._module_list.insert(std::make_pair(moduleContent, CPPPARSER::module(moduleContent)));
                    current_module = it.first;
                }

                /// 抽取 @sub_module 注释内容
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
            }
            else
            {
                std::cout << "没有注释" << std::endl;
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

void test_json();
int main(int argc, const char **argv)
{
    test_json();
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
    return Tool.run(newFrontendActionFactory<FunctionFrontendAction>().get());
}

void test_json()
{
    // create an empty structure (null)
    json j;

// add a number that is stored as double (note the implicit conversion of j to an object)
    j["pi"] = 3.141;

// add a Boolean that is stored as bool
    j["happy"] = true;

// add a string that is stored as std::string
    j["name"] = "Niels";

// add another null object by passing nullptr
    j["nothing"] = nullptr;

// add an object inside the object
    j["answer"]["everything"] = 42;

// add an array that is stored as std::vector (using an initializer list)
    j["list"] = { 1, 0, 2 };

// add another object (using an initializer list of pairs)
    j["object"] = { {"currency", "USD"}, {"value", 42.99} };
    std::cout << to_string(j) << std::endl;
}
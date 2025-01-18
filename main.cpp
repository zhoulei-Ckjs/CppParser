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
#include <clang/Frontend/DiagnosticRenderer.h>
#include <clang/Frontend/FrontendDiagnostic.h>
#include <clang/Basic/Diagnostic.h>

#include <nlohmann/json.hpp>
#include <filesystem>

#include "ExtractTools.h"
#include "CppParser.h"
#include "CommonHeader.h"

using namespace clang;
using namespace clang::tooling;
using json = nlohmann::json;

CPPPARSER::ErrorCode error_code = CPPPARSER::ErrorCode::Success;                   ///< 错误码
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
            if (FilePath.find(file_patch) == std::string::npos)
                return true;

            if (!Declaration->isCompleteDefinition()) {
                std::cout << "Warning: " << Declaration->getNameAsString() << " not fully defined!" << std::endl;
            } else {
                std::cout << Declaration->getNameAsString() << " is fully defined." << std::endl;
            }

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
                for (const auto *decl : Declaration->decls())
                {
                    if (const auto *field = llvm::dyn_cast<FieldDecl>(decl))
                    {
                        /// 获取并打印成员变量的注释
                        std::string comment_field_text;
                        if (const RawComment *field_raw_comment = _context->getRawCommentForDeclNoCache(field))
                            comment_field_text = field_raw_comment->getRawText(_context->getSourceManager()).str();
                        /// 提取 @name
                        std::string current_name = ExtractTools::ExtractNameContent(comment_field_text);
                        /// 提取 @brief
                        std::string current_brief = ExtractTools::ExtractBriefContent(comment_field_text);
                        auto current_field = current_class->second._field_list.find(field->getNameAsString());
                        if(current_field == current_class->second._field_list.end())
                        {
                            std::cout << "----------增加成员变量: " << field->getNameAsString() << std::endl;
                            current_class->second._field_list.insert(std::make_pair(field->getNameAsString(),
                                                                                    CPPPARSER::_field(Declaration->getNameAsString(),
                                                                                                      field->getType().getAsString(),
                                                                                                      field->getNameAsString(), field->getAccess(),
                                                                                                      current_name, current_brief)));
                        }
                    }
                    else if (const auto *var = llvm::dyn_cast<VarDecl>(decl))
                    {
                        if (var->isStaticDataMember())
                        {
                            /// 获取并打印成员变量的注释
                            std::string comment_var_text;
                            if (const RawComment *var_raw_comment = _context->getRawCommentForDeclNoCache(var))
                                comment_var_text = var_raw_comment->getRawText(_context->getSourceManager()).str();
                            /// 提取 @name
                            std::string current_name = ExtractTools::ExtractNameContent(comment_var_text);
                            /// 提取 @brief
                            std::string current_brief = ExtractTools::ExtractBriefContent(comment_var_text);
                            auto current_field = current_class->second._field_list.find(var->getNameAsString());
                            if(current_field == current_class->second._field_list.end())
                            {
                                std::cout << "----------增加静态成员变量: " << var->getNameAsString() << std::endl;
                                current_class->second._field_list.insert(std::make_pair(var->getNameAsString(),
                                                                                        CPPPARSER::_field(Declaration->getNameAsString(),
                                                                                                          var->getType().getAsString(),
                                                                                                          var->getNameAsString(), var->getAccess(),
                                                                                                          current_name, current_brief)));
                            }
                        }
                        else
                        {
                            std::cout << "----------增加非静态成员变量: " << var->getNameAsString() << std::endl;
                            error_code = CPPPARSER::ErrorCode::UnusualNotStatic;
                        }
                    }
                    else if (llvm::isa<CXXMethodDecl>(decl))
                    {
                        // 这是一个成员函数
//                        std::cout << "成员函数处理" << std::endl;
                    }
                    else if (llvm::isa<TypedefDecl>(decl))
                    {
                        // 这是一个 typedef 声明
//                        std::cout << "typedef处理" << std::endl;
                    }
                    else if (llvm::isa<UsingDecl>(decl))
                    {
                        // 这是一个 using 声明
//                        std::cout << "Using处理" << std::endl;
                    }
                    else
                    {
//                        std::cout << "----------增加未知成员变量: " << std::endl;
                    }
                }

                /// 6.获取类的所有方法
                for (auto it = Declaration->method_begin(); it != Declaration->method_end(); ++it)
                {
                    current_class->second._has_method = true;
                    CXXMethodDecl *method = *it;

                    /// 获取方法的注释
                    std::string current_func_comment;
                    if (const RawComment *method_raw_comment = _context->getRawCommentForDeclNoCache(method))
                        current_func_comment = method_raw_comment->getRawText(_context->getSourceManager()).str();

                    /// 抽取 @return 部分的注释
                    std::string current_return = ExtractTools::ExtractReturnContent(current_func_comment);
                    /// 抽取 @name 部分的注释
                    std::string current_func_name = ExtractTools::ExtractNameContent(current_func_comment);
                    /// 抽取 @brief 部分的注释
                    std::string current_func_brief = ExtractTools::ExtractBriefContent(current_func_comment);
                    auto current_func = current_class->second._method_list.find(method->getNameAsString());
                    if(current_func == current_class->second._method_list.end())
                    {
                        std::cout << "----------增加成员函数: " << method->getNameAsString() << std::endl;
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
                    if(current_func->second._param_comment_map.empty())
                    {
                        current_func->second._param_comment_map = ExtractTools::ExtractParamsContent(current_func_comment);
                        for(auto& iter : current_func->second._param_comment_map)
                        {
                            std::cout << "------------增加参数类型: " << std::endl;
                            std::cout << "-------------- " << iter.first << " : " << iter.second << std::endl;
                        }
                    }
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

    void HandleTranslationUnit(ASTContext &Context) override
    {
        _visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    ClassVisitor _visitor;
};
class IgnoreIncludeFileCallbacks : public clang::PPCallbacks {
public:
    explicit IgnoreIncludeFileCallbacks(clang::SourceManager &SM) : SM(SM) {}

    bool FileNotFound(clang::StringRef FileName) override
    {
        llvm::errs() << "Warning: " << FileName << " not found, ignoring.\n";
        return true;
    }

private:
    clang::SourceManager &SM;
};

class FunctionFrontendAction : public ASTFrontendAction
{
public:
    void ExecuteAction() override
    {
        clang::CompilerInstance &CI = getCompilerInstance();
        CI.getPreprocessor().addPPCallbacks(std::make_unique<IgnoreIncludeFileCallbacks>(CI.getSourceManager()));
        clang::ASTFrontendAction::ExecuteAction();
    }
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) override
    {
        /// 抑制所有诊断信息
//        CI.getDiagnostics().setSuppressAllDiagnostics(true);
//        CI.getDiagnostics().setClient(new MyDiagnosticConsumer, true);
        return std::make_unique<FunctionASTConsumer>(&CI.getASTContext());
    }
};

static llvm::cl::OptionCategory ToolingSampleCategory("tooling-sample");

void output_json_file(std::map<std::string, CPPPARSER::system>& system_list, const std::string& file_output)
{
    json j;

    /// 获取系统或服务
    for(auto& system : system_list)
    {
        j["systemList"].push_back(system.second.ToJson());
    }

    std::ofstream outFile(file_output);
//    outFile << std::setw(4) << j << std::endl;
    outFile << j << std::endl;
    outFile.close();
}

int WriteCompileCommand(const char* argv)
{
    auto my_path = std::filesystem::path(argv);
    std::cout << "输入文件夹路径为：" << absolute(my_path).string() << std::endl;
    /// 更新文件夹路径
    file_patch = absolute(my_path).string();
    if(!is_directory(my_path))
    {
        std::cout << argv << "不是文件夹，请提供文件夹!" << std::endl;
        return CPPPARSER::ErrorCode::NotDirectory;
    }

    json j;
    std::string command = " -I" + absolute(my_path).string();
    for(const auto& entry : std::filesystem::recursive_directory_iterator(absolute(my_path)))
    {
        if(entry.is_directory())
            command += " -I" + entry.path().string();
    }

    command = "/usr/bin/g++" + command + " -frtti -fexceptions -g -std=gnu++17 -fdiagnostics-color=always   -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS";

    int i = 0;
    for(const auto& entry : std::filesystem::recursive_directory_iterator(absolute(my_path)))
    {
        if(entry.is_regular_file() && entry.path().has_extension() && entry.path().extension().string() == ".h")
        {
            j[i]["directory"] = ".";
            j[i]["file"] = entry.path().string();
            j[i]["output"] = "/a.o";
            j[i]["command"] = command + " " + entry.path().string();
            i++;
        }
    }

    std::ofstream outFile("compile_commands.json");
    outFile << j.dump(4);
    outFile.close();
    return 0;
}

std::string GetOutputFile(const char *argv)
{
    auto my_path = std::filesystem::path(argv);
    std::cout << "输出文件为：" << absolute(my_path).string() << std::endl;
    return absolute(my_path).string();
}

void PreProcessFile(const std::filesystem::path& filePath)
{
    /// 读取文件内容
    std::ifstream inFile(filePath);
    if (!inFile.is_open())
    {
        std::cerr << "无法打开文件: " << filePath << std::endl;
        return;
    }

    std::string line;
    std::string fileContent;
    bool modified = false;

    /// 逐行读取文件内容
    while (std::getline(inFile, line))
    {
        auto to_be_substitute_it = g_header_substitute.begin();
        for(; to_be_substitute_it != g_header_substitute.end(); ++to_be_substitute_it)
        {
            if(ExtractTools::haveSuchContent(line, to_be_substitute_it->first))
            {
                /// 替换
                line = to_be_substitute_it->second;
                modified = true;
                break;
            }
        }
        fileContent += line + "\n";
    }

    inFile.close();

    /// 如果文件内容被修改，重新写入文件
    if (modified)
    {
        std::ofstream outFile(filePath);
        if (!outFile.is_open())
        {
            std::cerr << "无法写入文件: " << filePath << std::endl;
            return;
        }
        outFile << fileContent;
        outFile.close();
        std::cout << "修改了文件: " << filePath << std::endl;
    }
}

class MyDiagnosticConsumer : public DiagnosticConsumer
{
public:
    void HandleDiagnostic(DiagnosticsEngine::Level DiagLevel, const Diagnostic &Info) override
    {
        /// 捕获并处理诊断信息
        if (DiagLevel == DiagnosticsEngine::Error)
        {
            SmallString<100> DiagMessage;
            Info.FormatDiagnostic(DiagMessage);

            /// 检查是否是 "unknown type name" 错误
            if (StringRef(DiagMessage).starts_with("unknown type name"))
            {
                std::string unknown_class_name = ExtractTools::extractUnknownType(DiagMessage.c_str(), R"(unknown type name '([^']+)')");
                std::string key = R"(\s*)" + unknown_class_name + R"(\s*)";
                std::string value = "#ifndef __" + unknown_class_name + "__\n" +
                                    "#define __" + unknown_class_name + "__\n" +
                                    "class " + unknown_class_name + "{};\n" +
                                    "#endif\n";
                unknown_classes.insert(std::make_pair(key, value));
            }
            else if(StringRef(DiagMessage).starts_with("no template named"))
            {
                std::string unknown_class_name = ExtractTools::extractUnknownType(DiagMessage.c_str(), R"(no template named '([^']+)')");
                std::string key = R"(\s*)" + unknown_class_name + R"(\s*)";
                std::string value = "#ifndef __" + unknown_class_name + "__\n" +
                                    "#define __" + unknown_class_name + "__\n" +
                                    "template<typename... Args>\n" +
                                    "class " + unknown_class_name + "{};\n" +
                                    "#endif\n";
                unknown_template.insert(std::make_pair(key, value));
            }
            else if(StringRef(DiagMessage).starts_with("no type named") &&
                    StringRef(DiagMessage).contains(" in "))
            {
                std::string unknown_class_name = ExtractTools::extractUnknownType(DiagMessage.c_str(), R"(no type named '([^']+)')");
                std::string known_template = ExtractTools::extractUnknownType(DiagMessage.c_str(), R"( in '([^']+)')");
                std::string key = R"(\s*)" + unknown_class_name + R"(\s*)";
                std::string value = "\n#ifndef __" + unknown_class_name + "__\n" +
                                    "#define __" + unknown_class_name + "__\n" +
                                    "template<>\n" +
                                    "class " + known_template + "\n" +
                                    "{\n" +
                                    "public:\n" +
                                    "\tclass " + unknown_class_name + "{};\n" +
                                    "};\n" +
                                    "#endif\n";
                unknown_classes.insert(std::make_pair(key, value));
            }
            else if(StringRef(DiagMessage).starts_with("use of undeclared identifier"))
            {
                std::string unknown_class_name = ExtractTools::extractUnknownType(DiagMessage.c_str(), R"(use of undeclared identifier '([^']+)')");
                std::string key = R"(\s*)" + unknown_class_name + R"(\s*)";
                std::string value = "\n#ifndef __" + unknown_class_name + "__\n" +
                                    "#define __" + unknown_class_name + "__\n" +
                                    "class " + unknown_class_name + "\n" +
                                    "{\n" +
                                    "};\n" +
                                    "#endif\n";
                unknown_classes.insert(std::make_pair(key, value));
            }

            std::cout << DiagMessage.c_str() << std::endl;
        }

        /// 调用基类方法，以便其他诊断信息仍然能够正常处理
        DiagnosticConsumer::HandleDiagnostic(DiagLevel, Info);
    }
};

class DiagnosticASTConsumer : public ASTConsumer
{
public:
    explicit DiagnosticASTConsumer(ASTContext *Context)
    {}
};
class DiagnosticFrontendAction : public ASTFrontendAction
{
public:
    void ExecuteAction() override
    {
        clang::CompilerInstance &CI = getCompilerInstance();
        CI.getDiagnostics().setClient(new MyDiagnosticConsumer, true);
        CI.getPreprocessor().addPPCallbacks(std::make_unique<IgnoreIncludeFileCallbacks>(CI.getSourceManager()));
        clang::ASTFrontendAction::ExecuteAction();
    }
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) override
    {
        return std::make_unique<DiagnosticASTConsumer>(&CI.getASTContext());
    }
};
void GetAllUnknownClass(CommonOptionsParser &OptionsParser, std::vector<std::string>& allFiles)
{
    /// 使用 CommonOptionsParser 创建一个 ClangTool 实例。
    ClangTool Tool(OptionsParser.getCompilations(), allFiles);

    /// 运行 ClangTool，并使用 DiagnosticFrontendAction 处理输入文件。
    int ret = Tool.run(newFrontendActionFactory<DiagnosticFrontendAction>().get());
}


void PreProcessUnknownClassFile(std::string filePath)
{
    /// 读取文件内容
    std::ifstream inFile(filePath);
    if (!inFile.is_open())
    {
        std::cerr << "无法打开文件: " << filePath << std::endl;
        return;
    }

    std::string line;
    std::string fileContentPre;
    std::string fileContentPost;
    std::string toInsert;
    bool ParsingPreContent = true;
    bool modified = false;

    /// 逐行读取文件内容
    while (std::getline(inFile, line))
    {
        if(unknown_template.size() != 0)
        {
            auto to_be_substitute_it = unknown_template.begin();
            for(; to_be_substitute_it != unknown_template.end(); ++to_be_substitute_it)
            {
                if(ExtractTools::haveSuchContent(line, to_be_substitute_it->first))
                {
                    /// 替换
                    toInsert += to_be_substitute_it->second;
                    modified = true;
                }
            }
        }
        else
        {
            auto to_be_substitute_it = unknown_classes.begin();
            for(; to_be_substitute_it != unknown_classes.end(); ++to_be_substitute_it)
            {
                if(ExtractTools::haveSuchContent(line, to_be_substitute_it->first))
                {
                    /// 替换
                    toInsert += to_be_substitute_it->second;
                    modified = true;
                }
            }
        }

        if(ExtractTools::haveSuchContent(line, "#include"))
        {
            ParsingPreContent = false;
        }
        if(ParsingPreContent)
            fileContentPre += line + "\n";
        else
            fileContentPost += line + "\n";
    }

    inFile.close();

    /// 如果文件内容被修改，重新写入文件
    if (modified)
    {
        std::ofstream outFile(filePath);
        if (!outFile.is_open())
        {
            std::cerr << "无法写入文件: " << filePath << std::endl;
            return;
        }
        outFile << fileContentPre;
        outFile << toInsert;
        outFile << fileContentPost;
        outFile.close();
        std::cout << "修改了文件: " << filePath << std::endl;
    }
}
void PreProcessAllFile(const std::vector<std::string>& allFiles)
{
    for(auto& file : allFiles)
        PreProcessUnknownClassFile(file);
}

int main(int argc, const char **argv)
{
    if(argc < 3)
    {
        std::cout << "错误，输入参数不足，正确的输入示范为：" << std::endl;
        std::cout << "\t./FunctionParser <src_dir> <output_file>" << std::endl;
        return CPPPARSER::ErrorCode::NotEnoughParams;
    }

    /// 创建编译选项
    int ret = WriteCompileCommand(argv[1]);
    if(ret) return ret;

    /// 设置输出文件
    std::string output_file = GetOutputFile(argv[2]);

    /// 创建一个 CommonOptionsParser 实例，用于解析命令行选项。
    argc--;
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
                PreProcessFile(entry.path());
            }
        }
        std::cout << std::endl;
    }

    int i = 1;
    do
    {
        PreProcessAllFile(allFiles);
        unknown_classes.clear();
        unknown_template.clear();
        GetAllUnknownClass(OptionsParser, allFiles);
        std::cout << "-----------------------迭代次数[" << i << "], 未知类型个数[" << unknown_classes.size() << "]-----------------------" << std::endl;
    }while(unknown_classes.size() != 0 && i++ <= 10);

    std::cout << unknown_classes.size() << std::endl;

    /// 使用 CommonOptionsParser 创建一个 ClangTool 实例。
    ClangTool Tool(OptionsParser.getCompilations(), allFiles);

    /// 运行 ClangTool，并使用 FunctionFrontendAction 处理输入文件。
    ret = Tool.run(newFrontendActionFactory<FunctionFrontendAction>().get());

    output_json_file(systemList, output_file);
    return error_code;
}
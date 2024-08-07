#include <iostream>
#include <fstream>
#include <sstream>

#include <clang/AST/AST.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/ASTConsumers.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

#include "ExtractTools.h"

using namespace clang;
using namespace clang::tooling;

const char * file_papth = "/home/zl/CppParser";    ///< 只解析这个路径下的文件

class ClassVisitor : public RecursiveASTVisitor<ClassVisitor>
{
public:
    explicit ClassVisitor(ASTContext *Context) : Context(Context)
    {}

    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration)
    {
        FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
        if (FullLocation.isValid())
        {
            const SourceManager &SM = Context->getSourceManager();
            StringRef FilePath = SM.getFilename(FullLocation);
            if (FilePath.find(file_papth) == std::string::npos)
                return true;

            std::cout << "类: " << Declaration->getNameAsString();

            if (Declaration->isCompleteDefinition())
                std::cout << " 具有完整定义" << std::endl;
            else
                std::cout << " 不完全定义" << std::endl;

            if (const RawComment *Comment = Context->getRawCommentForDeclNoCache(Declaration))
            {
                std::string commentText = Comment->getRawText(Context->getSourceManager()).str();

                /// 抽取 @system 注释的内容
                std::string system_content = ExtractTools::ExtractSystemContent(commentText);
                std::cout << "\t@system: " << system_content << std::endl;
                /// 抽取 @module 注释内容
                std::string moduleContent = ExtractTools::ExtractModuleContent(commentText);
                std::cout << "\t@module: " << moduleContent << std::endl;
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
    ASTContext *Context;
};

class FunctionVisitor : public RecursiveASTVisitor<FunctionVisitor>
{
public:
    explicit FunctionVisitor(ASTContext *Context) : Context(Context)
    {}

    bool VisitFunctionDecl(FunctionDecl *Declaration)
    {
        FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
        if (FullLocation.isValid())
        {
            const SourceManager &SM = Context->getSourceManager();
            StringRef FilePath = SM.getFilename(FullLocation);
            if (FilePath.find(file_papth) == std::string::npos)
                return true;

            std::cout << "函数: " << Declaration->getNameInfo().getName().getAsString();

            if (Declaration->hasBody())
                std::cout << " 有定义" << std::endl;
            else
                std::cout << " 没有完整定义" << std::endl;

            if (const RawComment *Comment = Context->getRawCommentForDeclNoCache(Declaration))
            {
                std::string commentText = Comment->getRawText(Context->getSourceManager()).str();
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

private:
    ASTContext *Context;
};

class Visitor : public RecursiveASTVisitor<Visitor>
{
public:
    explicit Visitor(ASTContext *Context) : _function_visitor(Context), _class_visitor(Context)
    {}

    bool VisitFunctionDecl(FunctionDecl *Declaration)
    {
        return _function_visitor.VisitFunctionDecl(Declaration);
    }

    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration)
    {
        return _class_visitor.VisitCXXRecordDecl(Declaration);
    }

private:
    FunctionVisitor _function_visitor;
    ClassVisitor _class_visitor;
};

// consume AST
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
    Visitor _visitor;
};

class FunctionFrontendAction : public ASTFrontendAction
{
public:
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file)
    {
        return std::make_unique<FunctionASTConsumer>(&CI.getASTContext());
    }
};

static llvm::cl::OptionCategory ToolingSampleCategory("tooling-sample");

int main(int argc, const char **argv)
{
    /// 创建一个 CommonOptionsParser 实例，用于解析命令行选项。
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolingSampleCategory);
    if (!ExpectedParser)
    {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser &OptionsParser = *ExpectedParser;

    /// 使用 CommonOptionsParser 创建一个 ClangTool 实例。
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    /// 运行 ClangTool，并使用 FunctionFrontendAction 处理输入文件。
    return Tool.run(newFrontendActionFactory<FunctionFrontendAction>().get());
}

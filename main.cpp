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

using namespace clang;
using namespace clang::tooling;

class FunctionVisitor : public RecursiveASTVisitor<FunctionVisitor>
{
public:
    explicit FunctionVisitor(ASTContext *Context) : Context(Context) {}

    bool VisitFunctionDecl(FunctionDecl *Declaration)
    {
        FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
        if (FullLocation.isValid())
        {
            const SourceManager &SM = Context->getSourceManager();
            StringRef FilePath = SM.getFilename(FullLocation);
            if (FilePath.find("/home/zl/CLionProjects/main") == std::string::npos)
            {
                return true;
            }

            std::cout << "Function: " << Declaration->getNameInfo().getName().getAsString();

            if (Declaration->hasBody())
            {
                std::cout << " has body" << std::endl;
                if (const RawComment *Comment = Context->getRawCommentForDeclNoCache(Declaration))
                {
                    std::cout << "Comment: \n" << Comment->getRawText(Context->getSourceManager()).str();
                }
                std::cout << std::endl;
            }
            else
            {
                std::cout << " has no body" << std::endl;
                if (const RawComment *Comment = Context->getRawCommentForDeclNoCache(Declaration))
                {
                    std::cout << "Comment: \n" << Comment->getRawText(Context->getSourceManager()).str();
                }
                std::cout << std::endl;
            }
        }
        return true;
    }

private:
    ASTContext *Context;
};

// consume AST
class FunctionASTConsumer : public ASTConsumer
{
public:
    explicit FunctionASTConsumer(ASTContext *Context) : Visitor(Context) {}

    virtual void HandleTranslationUnit(ASTContext &Context)
    {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    FunctionVisitor Visitor;
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
    //创建一个 CommonOptionsParser 实例，用于解析命令行选项。
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolingSampleCategory);
    if (!ExpectedParser)
    {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser &OptionsParser = *ExpectedParser;

    //使用 CommonOptionsParser 创建一个 ClangTool 实例。
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    //运行 ClangTool，并使用 FunctionFrontendAction 处理输入文件。
    return Tool.run(newFrontendActionFactory<FunctionFrontendAction>().get());
}

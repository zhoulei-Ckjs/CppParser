#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

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

// Helper function to extract @module content
std::string extractModuleContent(const std::string &commentText)
{
    std::regex moduleRegex(R"(@module\s+(\S+))"); ///< 这是一个原始字符串字面量，表示正则表达式的内容。原始字符串字面量以 R"( 开始，并以 )" 结束，中间的内容不会对转义字符进行处理。
    std::smatch match;
    if (std::regex_search(commentText, match, moduleRegex))
    {
        return match[1].str();
    }
    return "";
}

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
            if (FilePath.find("/home/zl/CLionProjects/CppParser") == std::string::npos)
            {
                return true;
            }

            std::cout << "Class: " << Declaration->getNameAsString();

            if (Declaration->isCompleteDefinition())
            {
                std::cout << " is fully defined" << std::endl;
                if (const RawComment *Comment = Context->getRawCommentForDeclNoCache(Declaration))
                {
                    std::string commentText = Comment->getRawText(Context->getSourceManager()).str();
                    std::cout << "Comment: \n" << commentText;

                    // Extract @module content
                    std::string moduleContent = extractModuleContent(commentText);
                    std::cout << "\n@module: " << moduleContent << std::endl;
                }
                std::cout << std::endl;
            }
            else
            {
                std::cout << " is not fully defined" << std::endl;
                if (const RawComment *Comment = Context->getRawCommentForDeclNoCache(Declaration))
                {
                    std::string commentText = Comment->getRawText(Context->getSourceManager()).str();
                    std::cout << "Comment: \n" << commentText;

                    // Extract @module content
                    std::string moduleContent = extractModuleContent(commentText);
                    std::cout << "\n@module: " << moduleContent << std::endl;
                }
                std::cout << std::endl;
            }
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
            if (FilePath.find("/home/zl/CLionProjects/CppParser") == std::string::npos)
            {
                return true;
            }

            std::cout << "Function: " << Declaration->getNameInfo().getName().getAsString();

            if (Declaration->hasBody())
            {
                std::cout << " has body" << std::endl;
                if (const RawComment *Comment = Context->getRawCommentForDeclNoCache(Declaration))
                {
                    std::string commentText = Comment->getRawText(Context->getSourceManager()).str();
                    std::cout << "Comment: \n" << commentText;

                    // Extract @module content
                    std::string moduleContent = extractModuleContent(commentText);
                    std::cout << "\n@module: " << moduleContent << std::endl;
                }
                std::cout << std::endl;
            }
            else
            {
                std::cout << " has no body" << std::endl;
                if (const RawComment *Comment = Context->getRawCommentForDeclNoCache(Declaration))
                {
                    std::string commentText = Comment->getRawText(Context->getSourceManager()).str();
                    std::cout << "Comment: \n" << commentText;

                    // Extract @module content
                    std::string moduleContent = extractModuleContent(commentText);
                    std::cout << "\n@module: " << moduleContent << std::endl;
                }
                std::cout << std::endl;
            }
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

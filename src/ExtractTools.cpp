#include <regex>

#include "ExtractTools.h"

/**
 * @brief 抽取 brief 的内容
 * @param commentText 注释字符串
 * @return 注释中 @name 指定的内容
 */
std::string ExtractTools::ExtractBriefContent(const std::string& commentText)
{
    std::regex brief_regex(R"(@brief\s+(\S+))");
    std::smatch match;
    if(std::regex_search(commentText, match, brief_regex));
    {
        return match[1].str();
    }
    return "";
}

/**
 * @brief 抽取类内容
 * @param commentText 注释
 * @return 类注释中 @name 指定的内容
 */
std::string ExtractTools::ExtractNameContent(const std::string& commentText)
{
    std::regex class_regex(R"(@name\s+(\S+))");
    std::smatch match;
    if(std::regex_search(commentText, match, class_regex))
    {
        return match[1].str();
    }
    return "";
}

/**
 * @brief 抽取子模块
 * @param commentText
 * @return @sub_module的内容
 */
std::string ExtractTools::ExtractSubModuleContent(const std::string& commentText)
{
    std::regex subModuleRegex(R"(@sub_module\s+(\S+))");
    std::smatch match;
    if(std::regex_search(commentText, match, subModuleRegex))
    {
        return match[1].str();
    }
    return "";
}

std::string ExtractTools::ExtractModuleContent(const std::string &commentText)
{
    std::regex moduleRegex(R"(@module\s+(\S+))"); ///< 这是一个原始字符串字面量，表示正则表达式的内容。原始字符串字面量以 R"( 开始，并以 )" 结束，中间的内容不会对转义字符进行处理。
    std::smatch match;
    if (std::regex_search(commentText, match, moduleRegex))
    {
        return match[1].str();
    }
    return "";
}

std::string ExtractTools::ExtractSystemContent(const std::string &commentText)
{
    std::regex system_regex(R"(@system\s+(\S+))");
    std::smatch match;
    if(std::regex_search(commentText, match, system_regex))
    {
        return match[1].str();
    }
    return "";
}
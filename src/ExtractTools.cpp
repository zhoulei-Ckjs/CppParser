#include <regex>

#include "ExtractTools.h"

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
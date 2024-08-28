#include <regex>

#include "ExtractTools.h"


bool ExtractTools::haveSuchContent(const std::string& line, const std::string& content)
{
    std::regex content_regex(content);
    std::smatch match;
    if(std::regex_search(line, match, content_regex))
        return true;
    return false;
}

std::map<std::string, std::string> ExtractTools::ExtractParamsContent(const std::string& commentText)
{
    /// 正则表达式用于匹配 @param 后跟参数名和描述
    std::regex param_regex(R"(@param[ \t]+(\S+)\s+(.+))");
    std::sregex_iterator it(commentText.begin(), commentText.end(), param_regex);
    std::sregex_iterator end;

    std::map<std::string, std::string> param_map;
    for (; it != end; ++it)
    {
        /// (*it)[1] 是参数名
        /// (*it)[2] 是参数描述
        param_map[(*it)[1].str()] = (*it)[2].str();
    }
    return param_map;
}


std::string ExtractTools::ExtractReturnContent(const std::string& commentText)
{
    std::regex return_regex(R"(@return[ \t]+(\S+))");
    std::smatch match;
    if(std::regex_search(commentText, match, return_regex))
    {
        return match[1].str();
    }
    return "";
}

/**
 * @brief 抽取 brief 的内容
 * @param commentText 注释字符串
 * @return 注释中 @name 指定的内容
 */
std::string ExtractTools::ExtractBriefContent(const std::string& commentText)
{
    std::regex brief_regex(R"(@brief[ \t]+(\S+))");
    std::smatch match;
    if(std::regex_search(commentText, match, brief_regex))
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
    std::regex class_regex(R"(@name[ \t]+(\S+))");
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
    std::regex subModuleRegex(R"(@sub_module[ \t]+(\S+))");
    std::smatch match;
    if(std::regex_search(commentText, match, subModuleRegex))
    {
        return match[1].str();
    }
    return "";
}

std::string ExtractTools::ExtractModuleContent(const std::string &commentText)
{
    std::regex moduleRegex(R"(@module[ \t]+(\S+))"); ///< 这是一个原始字符串字面量，表示正则表达式的内容。原始字符串字面量以 R"( 开始，并以 )" 结束，中间的内容不会对转义字符进行处理。
    std::smatch match;
    if (std::regex_search(commentText, match, moduleRegex))
    {
        return match[1].str();
    }
    return "";
}

std::string ExtractTools::ExtractSystemContent(const std::string &commentText)
{
    std::regex system_regex(R"(@system[ \t]+(\S+))");
    std::smatch match;
    if(std::regex_search(commentText, match, system_regex))
    {
        return match[1].str();
    }
    return "";
}
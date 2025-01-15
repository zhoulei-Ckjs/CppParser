#ifndef CPPPARSER_EXTRACTTOOLS_H
#define CPPPARSER_EXTRACTTOOLS_H

#include <string>
#include <map>

class ExtractTools
{
public :
    /**
     * @brief 判断是否匹配
     * @param line 待匹配的行
     * @param to_regex 正则
     * @return 是/否
     */
    static std::string extractUnknownType(const std::string& line, const std::string& to_regex);
    /**
     * @brief 判断是否匹配
     * @param line 待匹配的行
     * @param content 正则
     * @return 是/否
     */
    static bool haveSuchContent(const std::string& line, const std::string& content);
    /**
     * @brief 提取 param 参数
     * @param commentText 注释字符串
     * @return param 和 解释
     */
    static std::map<std::string, std::string> ExtractParamsContent(const std::string& commentText);
    /**
     * @brief 获取 @return 的内容
     * @param commentText 注释字符串
     * @return 注释中 @return 指定的内容
     */
    static std::string ExtractReturnContent(const std::string& commentText);
    /**
     * @brief 抽取 brief 的内容
     * @param commentText 注释字符串
     * @return 注释中 @brief 指定的内容
     */
    static std::string ExtractBriefContent(const std::string& commentText);
    /**
     * @brief 抽取类内容
     * @param commentText 注释
     * @return 类注释中 @name 指定的内容
     */
    static std::string ExtractNameContent(const std::string& commentText);
    /**
     * @brief 抽取子模块
     * @param commentText
     * @return @sub_module的内容
     */
    static std::string ExtractSubModuleContent(const std::string& commentText);
    /**
     * @brief 抽取@module的内容
     * @param commentText 传入参数，注释
     * @return @module部分的内容
     */
    static std::string ExtractModuleContent(const std::string &commentText);
    /**
     * @brief 抽取 @system 的内容
     * @param commentText 传入的参数，注释
     * @return @system 的内容
     */
    static std::string ExtractSystemContent(const std::string &commentText);
};
#endif //CPPPARSER_EXTRACTTOOLS_H

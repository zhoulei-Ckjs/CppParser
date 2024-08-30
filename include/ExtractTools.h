#ifndef CPPPARSER_EXTRACTTOOLS_H
#define CPPPARSER_EXTRACTTOOLS_H

#include <string>

class ExtractTools
{
public :
    /**
     * @brief 抽取 brief 的内容
     * @param commentText 注释字符串
     * @return 注释中 @name 指定的内容
     */
    static std::string ExtractBriefContent(const std::string& commentText);
    /**
     * @brief 抽取类内容
     * @param commentText 注释
     * @return 类注释中 @name 指定的内容
     */
    static std::string ExtractClassContent(const std::string& commentText);
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

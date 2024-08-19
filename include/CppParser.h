#ifndef FUNCTIONPARSER_CPPPARSER_H
#define FUNCTIONPARSER_CPPPARSER_H

#include <map>
#include <vector>

namespace CPPPARSER
{
    /**
     * @brief 返回类型类
     */
    class returnType
    {
    public :
        std::string className;      ///< 类名
        std::string methodName;     ///< 方法名
        std::string paramType;      ///< 返回类型
        std::string name;           ///< @return指定的中文名
        std::string description;    ///< @return指定的中文名
        bool ignore;                ///< 是否忽略
    };

    /**
     * @brief 参数列表
     */
    class paramList
    {
    public:
        std::string className;      ///< 类名
        std::string methodName;     ///< 方法名
        std::string paramType;      ///< 参数类型
        std::string paramName;      ///< 参数的英文名
        std::string name;           ///< 类型对应的名称
        std::string description;    ///< 类型对应的描述
        bool ignore;                ///< 是否可以忽略
    };

    /**
     * @brief 方法列表
     */
    class methodList
    {
    public:
        std::string className;      ///< 类名
        std::string visibility;     ///< 可见性
        std::string methodName;     ///< 方法名
        std::vector<paramList> param_list;  ///< 参数列表
        returnType return_type;     ///< 返回类型
        std::map<std::string, std::string> paramCommentMap; ///< 参数comment map
        std::string returnTypeComment;      ///< @return
        std::string name;           ///< 方法名
        std::string description;    ///< 方法描述
        bool ignore;                ///< 是否忽略
    };

    /**
     * @brief 类属性列表
     */
    class fieldList
    {
    public :
        std::string className;      ///< 类名
        std::string fieldType;      ///< 属性类型
        std::string fieldName;      ///< 属性英文名
        std::string visibility;     ///< 可见性
        std::string name;           ///< @name
        std::string description;    ///< @description
        bool ignore;                ///< 是否忽略
    };

    /**
     * @brief 类信息类
     */
    class classList
    {
    public :
        std::string packageName;    ///< 中间件模块
        std::string className;      ///< 中间件名字
        std::vector<fieldList> field_list;  ///< 属性列表
        std::vector<methodList> method_list;///< 方法列表
        bool isInterface;           ///< 是否为接口
        bool isEnum;                ///< 是否为枚举
        bool isAnnotation;          ///< 是否为注解
        bool hasMethod;             ///< 是否有方法
        std::string system;         ///< 所属系统
        std::string module;         ///< 所属模块
        std::string subModule;      ///< 所属子模块
        std::string name;           ///< @name指定的中文类名
        std::string description;    ///< @brief指定的描述信息
        bool ignore;                ///< 是否忽略
    };

    /**
     * @brief 子模块类
     */
    class subModuleList
    {
    public :
        std::string name;           ///< @subModule指定的名字
        std::vector<classList> class_list;  ///< 类
    };

    /**
     * @brief 模块类
     */
    class moduleList
    {
    public :
        std::vector<subModuleList> sub_module_list; ///< 子模块集合
        std::string name;           ///< 模块名字@module
        std::vector<classList> class_list;  ///< 类
    };

    /**
     * @brief 系统类
     */
    class system
    {
    public :
        std::vector<moduleList> module_list;    ///< 模块列表
        std::string name;                       ///< 系统或服务名
    };
}

#endif //FUNCTIONPARSER_CPPPARSER_H

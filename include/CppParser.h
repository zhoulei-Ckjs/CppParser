#ifndef FUNCTIONPARSER_CPPPARSER_H
#define FUNCTIONPARSER_CPPPARSER_H

#include <map>
#include <vector>
#include <clang/Basic/Specifiers.h>

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
    class _field
    {
    public :
        explicit _field(std::string class_name, std::string field_type, std::string field_name,
                        clang::AccessSpecifier visibility, std::string name, std::string description,
                        bool ignore = false);
    public :
        std::string _class_name;    ///< 类名
        std::string _field_type;    ///< 属性类型
        std::string _field_name;    ///< 属性英文名
        std::string _visibility;    ///< 可见性
        std::string _name;          ///< @name
        std::string _description;   ///< @description
        bool _ignore;               ///< 是否忽略
    };

    /**
     * @brief 类信息类
     */
    class _class
    {
    public :
        explicit _class(std::string packageName, std::string class_name,
                        std::string system, std::string module, std::string sub_module, std::string name,
                        std::string description, bool ignore = false);

    public :
        std::string _packageName;   ///< 中间件模块
        std::string _class_name;    ///< 中间件名字
        std::map<std::string, _field> field_list;   ///< 属性列表
        std::vector<methodList> method_list;        ///< 方法列表
        /// TODO
        bool isInterface;           ///< 是否为接口
        bool isEnum;                ///< 是否为枚举
        bool isAnnotation;          ///< 是否为注解
        bool hasMethod;             ///< 是否有方法
        std::string _system;        ///< 所属系统
        std::string _module;        ///< 所属模块
        std::string _sub_module;    ///< 所属子模块
        std::string _name;          ///< @name指定的中文类名
        std::string _description;   ///< @brief指定的描述信息
        bool _ignore;                ///< 是否忽略
    };

    /**
     * @brief 模块类
     */
    class module
    {
    public :
        explicit module(std::string name);

    public :
        std::map<std::string, module> _sub_module_list;           ///< 子模块集合
        std::string _name;                                        ///< 模块名字@module
        std::map<std::string, _class> _class_list;                ///< 类
    };

    /**
     * @brief 系统类
     */
    class system
    {
    public :
        explicit system(std::string name);

    public:
        std::map<std::string, module> _module_list;       ///< 模块列表
        std::string _name;                                ///< 系统或服务名
    };
}

#endif //FUNCTIONPARSER_CPPPARSER_H

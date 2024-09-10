#include "CppParser.h"
#include <iostream>

namespace CPPPARSER
{
    returnType::returnType()
    {

    }
    returnType::returnType(std::string class_name, std::string method_name, std::string param_type,
                           std::string name, std::string description, bool ignore)
            : _class_name(class_name), _method_name(method_name), _param_type(param_type),
              _name(name), _description(description), _ignore(ignore)
    {
        std::cout << "--------------类名: " << _class_name << std::endl;
        std::cout << "--------------方法名: " << _method_name << std::endl;
        std::cout << "--------------返回类型: " << _param_type << std::endl;
        std::cout << "--------------中文名: " << _name << std::endl;
        std::cout << "--------------描述: " << _description << std::endl;
        std::cout << "--------------是否忽略: " << _ignore << std::endl;
    }
    json returnType::ToJson()
    {
        json j;
        j["className"] = _class_name;
        j["methodName"] = _method_name;
        j["paramType"] = _param_type;
        j["name"] = _name;
        j["description"] = _description;
        j["ignore"] = _ignore;
        return j;
    }

    _param::_param(std::string class_name, std::string method_name, std::string param_type,
                   std::string param_name, std::string name, std::string description, bool ignore)
            : _class_name(class_name), _method_name(method_name), _param_type(param_type),
              _param_name(param_name), _name(name), _description(description), _ignore(ignore)
    {
        std::cout << "--------------类名: " << class_name << std::endl;
        std::cout << "--------------方法名: " << method_name << std::endl;
        std::cout << "--------------参数类型: " << param_type << std::endl;
        std::cout << "--------------参数名: " << param_name << std::endl;
        std::cout << "--------------参数中文名: " << name << std::endl;
        std::cout << "--------------参数描述: " << description << std::endl;
        std::cout << "--------------是否忽略: " << ignore << std::endl;
    }

    json _param::ToJson()
    {
        json j;
        j["className"] = _class_name;
        j["methodName"] = _method_name;
        j["paramType"] = _param_type;
        j["paramName"] = _param_name;
        j["name"] = _name;
        j["description"] = _description;
        j["ignore"] = _ignore;
        return j;
    }

    _method::_method(std::string class_name, clang::AccessSpecifier visibility, std::string method_name,
                     std::string return_type_comment, std::string name, std::string description, bool ignore)
            : _class_name(class_name), _method_name(method_name), _return_type_comment(return_type_comment),
              _name(name), _description(description), _ignore(ignore)
    {
        switch (visibility)
        {
            case clang::AccessSpecifier::AS_private :
                _visibility = "Private";
                break;
            case clang::AccessSpecifier::AS_protected:
                _visibility = "Protected";
                break;
            default:
                _visibility = "Public";
                break;
        }

        std::cout << "------------类名: " << class_name << std::endl;
        std::cout << "------------可见性: " << _visibility << std::endl;
        std::cout << "------------成员函数名: " << method_name << std::endl;
        std::cout << "------------返回描述: " << return_type_comment << std::endl;
        std::cout << "------------成员函数中文名: " << name << std::endl;
        std::cout << "------------brief描述: " << description << std::endl;
    }
    json _method::ToJson()
    {
        json j;
        j["className"] = _class_name;
        j["visibility"] = _visibility;
        j["methodName"] = _method_name;
        for(auto& it : _param_list)
            j["paramList"].push_back(it.second.ToJson());
        j["returnType"] = _return_type.ToJson();
        for(auto& it : _param_comment_map)
        {
            json temp;
            temp[it.first] = it.second;
            j["paramCommentMap"].push_back(temp);
        }
        j["returnTypeComment"] = _return_type_comment;
        j["name"] = _name;
        j["description"] = _description;
        j["ignore"] = _ignore;
        return j;
    }

    _field::_field(std::string class_name, std::string field_type, std::string field_name,
                   clang::AccessSpecifier visibility, std::string name, std::string description, bool ignore)
            : _class_name(class_name), _field_type(field_type), _field_name(field_name),
              _name(name), _description(description), _ignore(ignore)
    {
        switch (visibility)
        {
            case clang::AccessSpecifier::AS_private :
                _visibility = "Private";
                break;
            case clang::AccessSpecifier::AS_protected:
                _visibility = "Protected";
                break;
            default:
                _visibility = "Public";
                break;
        }

        std::cout << "------------类名: " << class_name << std::endl;
        std::cout << "------------类型: " << field_type << std::endl;
        std::cout << "------------成员名: " << field_name << std::endl;
        std::cout << "------------可见性: " << _visibility << std::endl;
        std::cout << "------------成员的中文名: " << name << std::endl;
        std::cout << "------------描述: " << description << std::endl;
    }

    json _field::ToJson()
    {
        json j;
        j["className"] = _class_name;
        j["fieldType"] = _field_type;
        j["fieldName"] = _field_name;
        j["visibility"] = _visibility;
        j["name"] = _name;
        j["description"] = _description;
        j["ignore"] = _ignore;
        return j;
    }

    _class::_class(std::string packageName, std::string class_name,
                   std::string system, std::string module, std::string sub_module, std::string name,
                   std::string description, bool ignore)
            : _packageName(packageName), _class_name(class_name), _system(system),
              _module(module), _sub_module(sub_module), _name(name), _description(description),
              _ignore(ignore)
    {

    }

    json _class::ToJson()
    {
        json j;
        j["packageName"] = _packageName;
        j["className"] = _class_name;
        for(auto& it : _field_list)
            j["fieldList"].push_back(it.second.ToJson());
        for(auto& it : _method_list)
            j["methodList"].push_back(it.second.ToJson());
        j["isInterface"] = isInterface;
        j["isEnum"] = isEnum;
        j["isAnnotation"] = isAnnotation;
        j["hasMethod"] = _has_method;
        j["system"] = _system;
        j["module"] = _module;
        j["subModule"] = _sub_module;
        j["name"] = _name;
        j["description"] = _description;
        j["ignore"] = _ignore;
        return j;
    }

    module::module(std::string name) : _name(name)
    {

    }

    json module::ToJson()
    {
        json j;
        for(auto& it : _sub_module_list)
            j["subModuleList"].push_back(it.second.ToJson());
        j["name"] = _name;
        for(auto& it : _class_list)
            j["classList"].push_back(it.second.ToJson());
        return j;
    }

    system::system(std::string name) : _name(name)
    {

    }

    json system::ToJson()
    {
        json j;
        for(auto& it : _module_list)
            j["moduleList"].push_back(it.second.ToJson());
        j["name"] = _name;
        return j;
    }
}
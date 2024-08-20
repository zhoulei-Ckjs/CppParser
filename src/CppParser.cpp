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

    _class::_class(std::string packageName, std::string class_name,
            std::string system, std::string module, std::string sub_module, std::string name,
            std::string description, bool ignore)
            : _packageName(packageName), _class_name(class_name), _system(system),
              _module(module), _sub_module(sub_module), _name(name), _description(description),
              _ignore(ignore)
    {

    }

    module::module(std::string name) : _name(name)
    {

    }

    system::system(std::string name) : _name(name)
    {

    }
}
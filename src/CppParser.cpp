#include "CppParser.h"
#include <iostream>

namespace CPPPARSER
{
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
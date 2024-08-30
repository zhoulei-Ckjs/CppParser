#include "CppParser.h"

namespace CPPPARSER
{
    class_::class_(std::string packageName, std::string class_name,
                   std::string system, std::string module, std::string sub_module, std::string name,
                   std::string description)
            : _packageName(packageName), _class_name(class_name), _system(system),
              _module(module), _sub_module(sub_module), _name(name), _description(description)
    {
        ignore = false;
    }

    module::module(std::string name) : _name(name)
    {

    }

    system::system(std::string name) : _name(name)
    {

    }
}
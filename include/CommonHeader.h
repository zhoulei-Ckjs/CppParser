#ifndef FUNCTIONPARSER_COMMONHEADER_H
#define FUNCTIONPARSER_COMMONHEADER_H

#include <map>

std::map<std::string, std::string> g_header_substitute =
{
        {R"(\s*#include\s*<map>)", "namespace std{template<typename... Args> class map{};}"}
};

#endif //FUNCTIONPARSER_COMMONHEADER_H

#ifndef FUNCTIONPARSER_COMMONHEADER_H
#define FUNCTIONPARSER_COMMONHEADER_H

#include <map>

std::map<std::string, std::string> g_header_substitute =
{
        {R"(\s*#include\s*<list>)", R"(
#ifndef __ISCAS_STD_LIST__
#define __ISCAS_STD_LIST__
namespace std
{
    template<typename... Args>
    class list{};
}
#endif
)"},
        {R"(\s*#include\s*<map>)", R"(
#ifndef __ISCAS_STD_MAP__
#define __ISCAS_STD_MAP__
namespace std
{
    template<typename... Args>
    class map{};
}
#endif
)"},
        {R"(\s*#include\s*<pthread.h>)", R"(
#ifndef __ISCAS_PTHREAD_MUTEX_T__
#define __ISCAS_PTHREAD_MUTEX_T__
class pthread_mutex_t{};
class pthread_mutexattr_t{};
class pthread_rwlock_t{};
#endif
)"},
        {R"(\s*#include\s*<string>)", R"(
#ifndef __ISCAS_STRING__
#define __ISCAS_STRING__
namespace std
{
    class string{};
}
#endif
)"},
        {R"(\s*#include\s*<sys/time.h>)", R"(
#ifndef __ISCAS_SYS_TIME_H__
#define __ISCAS_SYS_TIME_H__
typedef struct timeval
{}timeval;
#endif
)"},
        {R"(\s*#include\s*<thread>)", R"(
#ifndef __ISCAS_STD_THREAD__
#define __ISCAS_STD_THREAD__
namespace std
{
    class thread{};
}
#endif
)"},
        {R"(\s*#include\s*<vector>)", R"(
#ifndef __ISCAS_STD_VECTOR__
#define __ISCAS_STD_VECTOR__
namespace std
{
    template<typename... Args>
    class vector{};
}
#endif
)"}
};

#endif //FUNCTIONPARSER_COMMONHEADER_H

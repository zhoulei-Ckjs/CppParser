#ifndef FUNCTIONPARSER_CLIENT_H
#define FUNCTIONPARSER_CLIENT_H

#include <list>
#include <map>
#include<pthread.h>
#include    <string>
#include<sys/time.h>

#include "UnknownClass.h"
#include "fjaljfldsja.h"

using namespace std;

#include "fjiojfja;fdsafd.h"

class Widget;

/**
 * @brief 客户端类，存储每个连接的客户端的元数据
 * @system 解析服务
 * @module 会话模块
 * @sub_module 客户端模块
 * @name 客户端类
 */
class Client
{
public :
    /**
     * @brief 发送数据包到客户端
     * @param buf 数据包
     * @param size 数据包长度
     * @return 成功或失败
     * @retval true 成功
     * @retval false 失败
     * @name 发送数据
     */
    bool Send(char* buf, int size);
    /**
     * @brief 测试string
     * @param test_string 测试字符串
     * @return 返回字符串
     */
    std::string testString(std::string test_string);
    /**
     * @brief 测试timeval
     * @param t 输入timeval
     * @return 返回timeval
     */
    timeval& testTimeval(const timeval& t);
    /**
     * @brief 测试UnknownFunc
     * @return UnknownCLassJ<UnknownClassK, float>::UnknownClassL& 引用
     * @param UnknownCLassM<UnknownClassN, float>::UnknownClassO 测试类型
     */
    UnknownCLassJ<UnknownClassK, float>::UnknownClassL& UnknownFunc(UnknownCLassM<UnknownClassN, float>::UnknownClassO);
private :
    /**
     * @brief 客户端套接子
     * @name 套接子
     */
    int fd;

    /**
     * @brief 测试窗口类
     * @name 窗口类
     */
    static Widget abc;

    /**
     * @brief 测试list
     * @name list测试
     */
    std::list<Client> test_list;
    /**
     * @brief 测试map
     * @name map 测试
     */
    map<Client*> test_map;
    /**
     * @brief 测试pthread_mutex_t
     * @name pthread_mutex_t测试
     */
    pthread_mutex_t test_pthread_mutex_t;
    /**
     * @brief 测试pthread_mutexattr_t
     * @brief pthread_mutexattr_t测试
     */
    pthread_mutexattr_t test_pthread_mutexattr_t;
    /**
     * @brief 测试pthread_rwlock_t
     * @name pthread_rwlock_t测试
     */
    pthread_rwlock_t test_pthread_rwlock_t;
    /**
     * @brief 测试string
     * @name string测试
     */
    string test_string;
    /**
     * @brief 测试timeval
     * @name timeval测试
     */
    timeval test_timeval;

    /**
     * @brief 未知类型
     * @name 类型未知
     */
    UnknownClass unknown_class;
    struct UnknownClassA unknown_classA;
    UnknownClassB<int, float> unkonwn_classB;
    UnknownClassC<UnknownClassD, int> unknown_classC;
    UnknownClassE<int>::UnknownClassF unknown_classE;
    UnknownCLassG<UnknownClassH, float>::UnknownClassI unknown_classG;
};

#endif //FUNCTIONPARSER_CLIENT_H

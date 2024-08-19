#ifndef FUNCTIONPARSER_CLIENTMANAGER_H
#define FUNCTIONPARSER_CLIENTMANAGER_H

#include <map>
#include "include/Session/Client.h"

/**
 * @brief 客户端管理类
 * @system 解析服务
 * @module 会话模块
 * @name 客户端管理类
 */
class ClientManager
{
public :
    /**
     * @brief 接受连接
     * @return 套接字
     */
    int Accept();
    /**
     * @brief 关闭链接
     * @param fd 套接字
     */
    void Close(int fd);

private :
    std::map<int, Client*> clients;     ///< 所有客户端
};

#endif //FUNCTIONPARSER_CLIENTMANAGER_H

#ifndef FUNCTIONPARSER_CLIENT_H
#define FUNCTIONPARSER_CLIENT_H

class Widget;

/**
 * @brief 客户端类
 * @system 解析服务
 * @module 会话模块
 * @sub_module
 * @name 客户端类
 */
class Client
{
public :
    /**
     * @brief 发送数据包到客户端
     * @param buf 数据包
     * @param size 数据包长度
     * @name 发送数据
     */
    void Send(char* buf, int size);
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
};

#endif //FUNCTIONPARSER_CLIENT_H

#ifndef FUNCTIONPARSER_MODELPROXY_H
#define FUNCTIONPARSER_MODELPROXY_H

/**
 * @brief 中间件类
 * @system 解析服务
 * @module 中间件模块
 * @name 中间件类
 */
class MiddleWare
{
public :
    /**
     * @brief 初始化
     * @return 成功或失败
     * @retval true 初始化成功
     * @retval false 初始化失败
     */
    bool Initialize();
    /**
     * @brief 下载数据
     */
    void PullMsg();

private:
    int param1;     ///< 成员1
    /// 成员2
    int param2;
};

#endif //FUNCTIONPARSER_MODELPROXY_H

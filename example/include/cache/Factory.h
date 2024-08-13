#ifndef FUNCTIONPARSER_FACTORY_H
#define FUNCTIONPARSER_FACTORY_H

#include "include/cache/Object.h"

/**
 * @brief 工厂类
 * @system 解析服务
 * @module 缓存模块
 * @name 工厂类
 */
class Factory
{
public :
    /**
     * @brief 创建实例
     * @param type 类型
     * @return 创建对象指针
     */
    Object* Create(const int type);
    /**
     * @brief 获取实例
     * @param type 类型
     * @return 获取对象指针
     */
    Object* Get(int type);
};

#endif //FUNCTIONPARSER_FACTORY_H

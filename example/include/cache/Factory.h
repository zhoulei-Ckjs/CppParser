/**
 * @file example.h
 * @brief example.h文件简述
 *
 * 这个文件是XXX项目的一部分。\n
 * 请勿随意更改、复制或分发、部署。\n
 * 如有需求，请联系中国科学院软件研究所。
 * @email iscas\@ac.cn
 * @copyright 2024 ISCAS
 * @author iscas
 * @version 0.0.1.20241113
 */

#ifndef FUNCTIONPARSER_FACTORY_H
#define FUNCTIONPARSER_FACTORY_H

#include "include/cache/Object.h"

/**
 * @brief 工厂类，依据对象类型创建对象。
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

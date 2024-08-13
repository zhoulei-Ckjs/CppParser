#ifndef FUNCTIONPARSER_OBJECT_H
#define FUNCTIONPARSER_OBJECT_H

/**
* @brief 对象鸡类
* @system 解析服务
* @module 缓存模块
* @name 对象鸡类
*/
class Object
{
public :
    /**
     * @brief 更新对象
     * @param buf 对象缓存
     */
    virtual void Update(void* buf) = 0;
    /**
     * @brief 编码
     * @param code 待编码数据
     */
    virtual void Encode(int& code) = 0;
    /**
     * @brief 解码
     * @param code 待解码数据
     */
    virtual void Decode(int& code) = 0;

public:
    /// 对象数据1
    int object_1;
    int object_2;       ///< 对象数据2
};

#endif //FUNCTIONPARSER_OBJECT_H

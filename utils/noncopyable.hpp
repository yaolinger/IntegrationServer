#ifndef UTILS_NONCOPYABLE_HPP
#define UTILS_NONCOPYABLE_HPP

#include "macros.hpp"

NS_UTILS_BEGIN

/* 禁止拷贝基础类
*  禁止拷贝构造
*  禁止赋值拷贝操作服(无法禁止子类重新实现)
*/
class Noncopyable {
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
private:
    Noncopyable(const Noncopyable&);

    const Noncopyable& operator = (const Noncopyable&);
};

NS_UTILS_END

#endif

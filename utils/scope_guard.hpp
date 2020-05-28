#ifndef UTILS_SCOPE_GUARD_HPP
#define UTILS_SCOPE_GUARD_HPP

#include "macros.hpp"
#include "noncopyable.hpp"

NS_UTILS_BEGIN

// func执行包装器
// RAII机制
// 出作用域 析构自动调用func
template <typename Func>
class ScopeGuard : public Noncopyable {
public:
    ScopeGuard(Func f) : m_func(std::move(f)), m_active(true) {}
    ScopeGuard(ScopeGuard&& sg) : m_func(std::move(sg.m_func)), m_active(sg.m_active) {}
    ~ScopeGuard() { if (m_active) m_func(); }

public:
    // 放弃执行
    void dismiss() { m_active = false; }

private:
    Func m_func;
    bool m_active;
};

template <typename Func>
ScopeGuard<Func> NewScopeGuard(Func f) {
    return ScopeGuard<Func>(std::move(f));
}

NS_UTILS_END

#endif

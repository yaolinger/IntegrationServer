#ifndef TEST_SYNTAX_TEST_SFINAE_HPP
#define TEST_SYNTAX_TEST_SFINAE_HPP

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"

// SFINAE:
// 匹配失败不是错误(Substitution Failure Is Not An Error)
// 第一层目的:匹配模板失败则去匹配下一个只要有正确模板就不会编译报错
// 第二层目的:可以使函数利用模板支持相同性质不同类型的参数, 如同时支持仿函数和普通函数, 不必要写两个实现
// 第三层目的:兼容合理的推测报错, 用断言报错, 降低编译错误理解
// 经典场景:boost::asio::socket::async_read_some, async_send

// 返回值
typedef char result;   // 1字节
typedef uint16 result2;  // 2字节

// 检测参数模板
// 注意 逗号表达式以及, decltyp推导
// char(0) 为把0 int转化为char类型， 修改为cpp版本强制转换, 容易理解一些
template <typename Handler, typename Arg1>
auto oneArgHandlerTest(Handler h, Arg1 a) -> decltype(h(a), static_cast<result>(0));

template <typename Handler>
result2 oneArgHandlerTest(Handler h, ...);

void TestSFINAE() {
    FUNC_TRACE;
    USING_NS_UTILS;

    {
        log_info("********************[小知识]***************************");
        log_info("sizeof(表达式)不会执行表达式,但是会给出表达式结果的大小");
        std::string str;
        log_info("Str size[%u]", sizeof(str = "牛批了兄弟"));
        log_info("Str[%s]", str.c_str());
        log_info("*******************************************************");
    }

    // 仿函数(1参数)
    class Functor {
    public:
        void operator () (std::string error) {
        }
    };
    Functor oneArgFunctor;
    // 函数(1参数)
    auto oneArgFunc = [](std::string error) {
    };
    // 函数(无参数)
    auto nullArgFunc = []() {
    };

    std::string error;
    static_assert(sizeof(oneArgHandlerTest(oneArgFunc, error)) == 1, "ArgHandler not met.");
    static_assert(sizeof(oneArgHandlerTest(oneArgFunctor, error)) == 1, "ArgHandler not met.");

    // 模板推导为 result2 oneArgHandlerTest sizeof == 2, 断言1则编译报出异常
    //static_assert(sizeof(oneArgHandlerTest(nullArgFunc, error)) == 1, "ArgHandler not met.");
    static_assert(sizeof(oneArgHandlerTest(nullArgFunc, error)) == 2, "ArgHandler not met.");
}

#endif

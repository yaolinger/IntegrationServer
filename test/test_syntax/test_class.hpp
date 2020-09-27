#ifndef TEST_SYNTAX_TEST_CLASS
#define TEST_SYNTAX_TEST_CLASS

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"

// 奇怪的语法测试
// 静态成员测试
class TestStatic {
    public:
        static uint32 n;
        static const uint32 i = 2;  // 很独特 类内静态常量整形成员 可以类内初始化
        static const std::string str;
        static const double f;
};

uint32 TestStatic::n = 6;
const std::string TestStatic::str = "小母牛做飞机";
const double TestStatic::f = 3.2;

void TestClass() {
    FUNC_TRACE;
    USING_NS_UTILS;
    log_info("Static n[%u] i[%u] str[%s] f[%f]", TestStatic::n, TestStatic::i, TestStatic::str.c_str(), TestStatic::f);
}

#endif

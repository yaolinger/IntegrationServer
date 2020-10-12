#ifndef TEST_SYNTAX_TEST_MUTEX
#define TEST_SYNTAX_TEST_MUTEX

#include <mutex>

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"

// <mutex>测试

void TestMutex() {
    FUNC_TRACE;
    USING_NS_UTILS;

    {
        log_warn("****************std::call_once***************");
        std::once_flag flag1;
        uint32 times = 0;
        auto func = [&times]() {
            log_info("Func run times[%u]", ++times);
        };

        std::call_once(flag1, func);
        std::call_once(flag1, func);
        std::call_once(flag1, func);
    }

}

#endif

#ifndef TEST_UTILS_TEST_RAND_HPP
#define TEST_UTILS_TEST_RAND_HPP

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"
#include "utils/rand.hpp"

void TestRand() {
    FUNC_TRACE;
    USING_NS_UTILS;

    for (uint32 i = 0; i < 10; i++) {
        log_debug("Rand[%d]", Rand::randBetween((int32)6, (int32)5));
    }

    std::string str =  Rand::randString(12);
    log_debug("Rand str[%s]", str.c_str());

}

#endif

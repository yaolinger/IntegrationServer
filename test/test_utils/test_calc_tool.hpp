#ifndef TEST_UTILS_TEST_CALC_TOOL_HPP
#define TEST_UTILS_TEST_CALC_TOOL_HPP

#include <string>
#include "utils/calc_tool.hpp"
#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"

void TestCalcTool() {
    FUNC_TRACE;
    USING_NS_UTILS;

    std::string str("-1234");
    int32 num32 = string_to_int32(str);
    log_debug("int32[%d]", num32);

    uint32 uNum32 = string_to_uint32(str);
    log_debug("uint32[%u]", uNum32);

    str = std::string("23456");
    num32 = string_to_int32(str);
    log_debug("int32[%d]", num32);

    str = std::string("-7000000000");
    int64 num64 = string_to_int64(str);
    log_debug("int64[%ld]", num64);

    uint64 uNum64 = string_to_uint64(str);
    log_debug("uint64[%lu]", uNum64);

    str = std::string("7000000000");
    num64 = string_to_int64(str);
    log_debug("int64[%ld]", num64);

    str = std::string("18446744073709551615");
    uNum64 = string_to_uint64(str);
    log_debug("uint64[%lu]", uNum64);

    str = std::string("ashjhjkfhajk");
    uNum64 = string_to_uint64(str);
    log_debug("uint64[%lu]", uNum64);

    uNum32 = string_to_uint32(str);
    log_debug("uint32[%u]", uNum32);
}

#endif

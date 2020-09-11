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
    int32 num32 = stringToInt32(str);
    log_debug("int32[%d]", num32);

    uint32 uNum32 = stringToUint32(str);
    log_debug("uint32[%u]", uNum32);

    str = std::string("23456");
    num32 = stringToInt32(str);
    log_debug("int32[%d]", num32);

    str = std::string("-7000000000");
    int64 num64 = stringToInt64(str);
    log_debug("int64[%ld]", num64);

    uint64 uNum64 = stringToUint64(str);
    log_debug("uint64[%lu]", uNum64);

    str = std::string("7000000000");
    num64 = stringToInt64(str);
    log_debug("int64[%ld]", num64);

    str = std::string("18446744073709551615");
    uNum64 = stringToUint64(str);
    log_debug("uint64[%lu]", uNum64);

    str = std::string("ashjhjkfhajk");
    uNum64 = stringToUint64(str);
    log_debug("uint64[%lu]", uNum64);

    uNum32 = stringToUint32(str);
    log_debug("uint32[%u]", uNum32);

    log_warn("**************割割割***************");
    {
        uint32 param1 = 7;
        uint32 param2 = 10;
        log_info("uint32:param1[%u] - param2[%u] = [%u]", param1, param2, safeSub(param1, param2));
    }
    {
        uint64 param1 = 32;
        uint64 param2 = 64;
        log_info("uint64:param1[%lu] - param2[%lu] = [%lu]", param1, param2, safeSub(param1, param2));
    }
    {
        int32 param1 = -3;
        int32 param2 = 10;
        log_info("int32:param1[%d] - param2[%d] = [%d]", param1, param2, safeSub(param1, param2));
    }
    {
        int64 param1 = 19;
        int64 param2 = 17;
        log_info("int64:param1[%ld] - param2[%ld] = [%ld]", param1, param2, safeSub(param1, param2));
    }
}

#endif

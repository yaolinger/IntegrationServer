#include "calc_tool.hpp"

#include <stdexcept>

NS_UTILS_BEGIN

int32 stringToInt32(const std::string& str) {
    int32 result = 0;
    try {
        result = std::stoi(str);
    } catch (const std::invalid_argument& error) {
        result = 0;
    }
    return result;
}

int64 stringToInt64(const std::string& str) {
    int64 result = 0;
    try {
        result = std::stoll(str);
    } catch (const std::invalid_argument& error) {
        result = 0;
    }
    return result;
}

uint32 stringToUint32(const std::string& str) {
    uint32 result = 0;
    try {
        result = std::stoul(str);
    } catch (const std::invalid_argument& error) {
        result = 0;
    }
    return result;
}

uint64 stringToUint64(const std::string& str) {
    uint64 result = 0;
    try {
        result = std::stoull(str);
    } catch (const std::invalid_argument& error) {
        result = 0;
    }
    return result;
}

template<>
uint32 safeSub(uint32 param1, uint32 param2) {
    if (param1 >= param2) {
        return param1 - param2;
    }
    return 0;
}

template<>
uint64 safeSub(uint64 param1, uint64 param2) {
    if (param1 >= param2) {
        return param1 - param2;
    }
    return 0;
}

template<>
int32 safeSub(int32 param1, int32 param2) {
    return param1 - param2;
}

template<>
int64 safeSub(int64 param1, int64 param2) {
    return param1 - param2;
}

NS_UTILS_END

#include "calc_tool.hpp"

#include <stdexcept>

NS_UTILS_BEGIN

int32 string_to_int32(const std::string& str) {
    int32 result = 0;
    try {
        result = std::stoi(str);
    } catch (const std::invalid_argument& error) {
        result = 0;
    }
    return result;
}

int64 string_to_int64(const std::string& str) {
    int64 result = 0;
    try {
        result = std::stoll(str);
    } catch (const std::invalid_argument& error) {
        result = 0;
    }
    return result;
}

uint32 string_to_uint32(const std::string& str) {
    uint32 result = 0;
    try {
        result = std::stoul(str);
    } catch (const std::invalid_argument& error) {
        result = 0;
    }
    return result;
}

uint64 string_to_uint64(const std::string& str) {
    uint64 result = 0;
    try {
        result = std::stoull(str);
    } catch (const std::invalid_argument& error) {
        result = 0;
    }
    return result;
}

NS_UTILS_END

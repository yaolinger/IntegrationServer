#ifndef UTILS_CALC_TOOL_HPP
#define UTILS_CALC_TOOL_HPP

#include <string>

#include "macros.hpp"

NS_UTILS_BEGIN

// 字符串转数字
int32 stringToInt32(const std::string& str);

int64 stringToInt64(const std::string& str);

uint32 stringToUint32(const std::string& str);

uint64 stringToUint64(const std::string& str);

// 安全减法
template<typename T>
T safeSub(T param1, T param2);

template<>
uint32 safeSub(uint32 param1, uint32 param2);

template<>
uint64 safeSub(uint64 param1, uint64 param2);

template<>
int32 safeSub(int32 param1, int32 param2);

template<>
int64 safeSub(int64 param1, int64 param2);

NS_UTILS_END

#endif

#ifndef UTILS_CALC_TOOL_HPP
#define UTILS_CALC_TOOL_HPP

#include <string>

#include "macros.hpp"

NS_UTILS_BEGIN

int32 string_to_int32(const std::string& str);

int64 string_to_int64(const std::string& str);

uint32 string_to_uint32(const std::string& str);

uint64 string_to_uint64(const std::string& str);

NS_UTILS_END

#endif

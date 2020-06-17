#ifndef UTILS_RAND_HPP
#define UTILS_RAND_HPP

#include "macros.hpp"

#include <random>
#include <string>
#include <vector>

NS_UTILS_BEGIN

// 随机数对象
class Rand {
public:
    static uint32 getRandSeed() { return s_rd(); }

    // 获取随机值[min, max] 包括min max
    template <class T>
    static T randBetween(T min, T max);

    // 获取随机字符串(英文+数字)
    static std::string randString(uint32 length);

private:
    static std::random_device s_rd;      // 随机数种子
};

template<>
int32 Rand::randBetween(int32 min, int32 max);

template<>
int64 Rand::randBetween(int64 min, int64 max);

template<>
uint32 Rand::randBetween(uint32 min, uint32 max);

template<>
uint64 Rand::randBetween(uint64 min, uint64 max);

NS_UTILS_END

#endif

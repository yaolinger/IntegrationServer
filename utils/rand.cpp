#include "rand.hpp"

#include "log.hpp"

NS_UTILS_BEGIN

std::random_device Rand::s_rd;

static const char engArray[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

static const char numArray[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

static const uint32 engArrayLen = sizeof(engArray);

static const uint32 numArrayLen = sizeof(numArray);

static const uint32 allCharLen = engArrayLen + numArrayLen;

std::string Rand::randString(uint32 length) {
    std::string result;
    for (uint i = 0; i < length; i++) {
        uint32 index = randBetween((uint32)0, allCharLen - 1);
        if (index < engArrayLen) {
            result.push_back(engArray[index]);
        } else {
            result.push_back(numArray[index - engArrayLen]);
        }
    }
    return result;
}

template<>
int32 Rand::randBetween(int32 min, int32 max) {
    if (min >= max) {
        return max;
    }
    std::mt19937 eng(Rand::getRandSeed());
    std::uniform_int_distribution<int32> dis(min, max);
    return dis(eng);
}

template<>
int64 Rand::randBetween(int64 min, int64 max) {
    if (min >= max) {
        return max;
    }
    std::mt19937 eng(Rand::getRandSeed());
    std::uniform_int_distribution<int64> dis(min, max);
    return dis(eng);
}

template<>
uint32 Rand::randBetween(uint32 min, uint32 max) {
    if (min >= max) {
        return max;
    }
    std::mt19937 eng(Rand::getRandSeed());
    std::uniform_int_distribution<uint32> dis(min, max);
    return dis(eng);
}

template<>
uint64 Rand::randBetween(uint64 min, uint64 max) {
    if (min >= max) {
        return max;
    }
    std::mt19937 eng(Rand::getRandSeed());
    std::uniform_int_distribution<uint64> dis(min, max);
    return dis(eng);
}



NS_UTILS_END

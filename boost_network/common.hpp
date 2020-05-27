#ifndef BOOST_NETWORK_COMMON_HPP
#define BOOST_NETWORK_COMMON_HPP

#include "utils/macros.hpp"

// 数据头包(1字节对齐)
#pragma pack(1)
struct MsgHead {
	uint16 cmd;          // 协议号
	uint16 len;          // 数据长度
	uint8 index;         // 包索引
	uint8 flag;          // 数据标识
};
// 恢复常规内存对齐
#pragma pack()

// 数据标识
enum PACKET_FLAG {
	PACKET_FLAG_ENCRYPT = 0x01,   // 数据包加密  00000001
	PACKET_FLAG_ZIP = 0x02,       // 数据包压缩  00000010
    PACKET_FLAG_UID = 0x04,       // 携带uid     00000100
};

// 动态缓存区基础buffer长度
#define DYNAMIC_BUFF_BASE_SIZE 64 * 1024
// 动态扩展倍数计算式
#define DYNAMIC_BUFF_EXPAND_TIMES(size) (size + DYNAMIC_BUFF_BASE_SIZE -1) / DYNAMIC_BUFF_BASE_SIZE
// 静态缓存区buffer长度
#define STATIC_BUFF_MAX_SIZE 64 * 10 * 1024

// 数据包最大长度(65535)
static const uint32 PACKET_MAX_SIZE = 64 * 1024 - 1;
// 头包长度
static const uint32 PACKET_HEAD_SIZE = sizeof(MsgHead);
// 包体最大长度
static const uint32 PACKET_BODY_MAX_SIZE = PACKET_MAX_SIZE - PACKET_HEAD_SIZE;

// 数据包长度小于此值 压缩无效
static const uint32 PACKET_COMPRESS_MIN_SIZE = 128;
// 未压缩数据最大长度
// 超过此值无法压缩(1M 压缩至 60k左右)
static const uint32 PACKET_UNCOMPRESS_MAX_SIZE = 1024 * 1024;
#endif

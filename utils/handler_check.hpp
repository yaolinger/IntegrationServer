#ifndef UTILS_HANDLER_CHECK_HPP
#define UTILS_HANDLER_CHECK_HPP

#include "macros.hpp"

NS_UTILS_BEGIN

// 返回值字节(重命名,清晰点)
typedef int8 byte;    // 1 byte
typedef int16 byte2;  // 2 byte

/***********************************
 * c++技巧SFINAE(匹配失败不是错误)
 * 配合SchedulerWrappedHandler
 * 目前最多支持6个参数哦(●'◡'●)
 */

// 0参数handler校验
template <typename Handler>
auto zeroArgHandlerCheck(Handler h) -> decltype(h(), byte(0));

template <typename Handler>
byte2 zeroHandlerCheck(Handler h, ...);

// 1参数handler校验
template <typename Handler, typename Arg1>
auto oneArgHandlerCheck(Handler h, Arg1 a1) -> decltype(h(a1), byte(0));

template <typename Handler>
byte2 oneArgHandlerCheck(Handler h, ...);

// 2参数handler校验
template <typename Handler, typename Arg1, typename Arg2>
auto twoArgHandlerCheck(Handler h, Arg1 a1, Arg2 a2) -> decltype(h(a1, a2), byte(0));

template <typename Handler>
bytes twoArgHandlerCheck(Handler h, ...);

// 3参数handler校验
template <typename Handler, typename Arg1, typename Arg2, typename Arg3>
auto threeArgHandlerCheck(Handler h, Arg1 a1, Arg2 a2, Arg3 a3) -> decltype(h(a1, a2, a3), byte(0));

template <typename Handler>
bytes threeArgHandlerCheck(Handler h, ...);

// 4参数handler校验
template <typename Handler, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
auto fourArgHandlerCheck(Handler h, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4) -> decltype(h(a1, a2, a3, a4), byte(0));

template <typename Handler>
bytes fourArgHandlerCheck(Handler h, ...);

// 5参数handler校验
template <typename Handler, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
auto fiveArgHandlerCheck(Handler h, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5) -> decltype(h(a1, a2, a3, a4, a5), byte(0));

template <typename Handler>
bytes fiveArgHandlerCheck(Handler h, ...);

// 6参数handler校验
template <typename Handler, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
auto sixArgHandlerCheck(Handler h, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6) -> decltype(h(a1, a2, a3, a4, a5, a6), byte(0));

template <typename Handler>
bytes sixArgHandlerCheck(Handler h, ...);

NS_UTILS_END

#endif

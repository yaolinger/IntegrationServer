#ifndef UTILS_HANDLER_CHECK_SOCKET_HPP
#define UTILS_HANDLER_CHECK_SOCKET_HPP

#include <functional>
#include <string>

#include "handler_check.hpp"

NS_UTILS_BEGIN

// 异步read模板参数检测
#define ASYNC_READ_HANDLER_CHECK(Handler) \
    static_assert(sizeof(twoArgHandlerCheck(Handler, \
                    static_cast<const std::string>(""), \
                    static_cast<int32>(0))) == 1, \
            "AsyncReadHandler args must met (const std::string error, uint32 transBytes)");

// 异步write模板参数检测
#define ASYNC_WRITE_HANDLER_CHECK(Handler) \
    static_assert(sizeof(twoArgHandlerCheck(Handler, \
                    static_cast<const std::string>(""), \
                    static_cast<int32>(0))) == 1, \
            "AsyncWriteHandler args must met (const std::string error, uint32 transBytes)");

// 异步connect模板参数检测
#define ASYNC_CONNECT_HANDLER_CHECK(Handler) \
    static_assert(sizeof(oneArgHandlerCheck(Handler, \
                    static_cast<const std::string>(""))) == 1, \
            "AsyncConnect args must met (const std::string error)");

// 异步accept模板参数检测
#define ASYNC_ACCEPT_HANDLER_CHECK(Handler) \
    static_assert(sizeof(oneArgHandlerCheck(Handler, \
                    static_cast<const std::string>(""))) == 1, \
            "AsyncAccept args must met (const std::string error)");

NS_UTILS_END

#endif


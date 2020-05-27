#ifndef BOOST_NETWORK_IO_INTERFACE_HPP
#define BOOST_NETWORK_IO_INTERFACE_HPP

#include <memory>

#include <boost/asio/io_service.hpp>

#include "message.hpp"

NS_BOOST_NETWORK_BEGIN

class TcpSocket;
typedef std::shared_ptr<TcpSocket> TcpSocketPtr;

// io回调接口
class IoCallback {
public:
    IoCallback() {}
    virtual ~IoCallback() {}

    virtual const char * name() = 0;
    virtual void onNewAccept(const TcpSocketPtr& s) = 0;
    virtual void onNewMsg(const TcpSocketPtr& s, uint16 cmd, const MsgBufPtr& buf) = 0;
    virtual void onConnect(const TcpSocketPtr& s) = 0;
    virtual void onClose(const TcpSocketPtr& s) = 0;
};

// acceptor回调
class AcceptorCallback : public IoCallback {
public:
    AcceptorCallback() {}
    ~AcceptorCallback() {}

    virtual const char * name() = 0;
    virtual void onNewAccept(const TcpSocketPtr& s) = 0;
    virtual void onNewMsg(const TcpSocketPtr& s, uint16 cmd, const MsgBufPtr& buf) = 0;
    virtual void onClose(const TcpSocketPtr& s) = 0;

    // onConnect 不需要
    void onConnect(const TcpSocketPtr& s) {}
};

// connect回调
class ConnectorCallback : public IoCallback {
public:
    ConnectorCallback() {}
    ~ConnectorCallback() {}

    virtual const char * name() = 0;
    virtual void onNewMsg(const TcpSocketPtr& s, uint16 cmd, const MsgBufPtr& buf) = 0;
    virtual void onConnect(const TcpSocketPtr& s) = 0;
    virtual void onClose(const TcpSocketPtr& s) = 0;

    //onNewAccrpt 不需要
    void onNewAccept(const TcpSocketPtr& s) {}
};

// io接口(基础)
class IoInterface {
public:
    explicit IoInterface(boost::asio::io_service& ios, IoCallback *pIoCallback) : m_ios(ios), m_ioCallback(pIoCallback) {}

    ~IoInterface() {}

public:
    virtual void onReadError(const TcpSocketPtr& s, const boost::system::error_code& ec) = 0;

    virtual void onWriteError(const TcpSocketPtr& s, const boost::system::error_code& ec) = 0;

    virtual void onSocketClose(const TcpSocketPtr& s) = 0;

    void onNewMsg(const TcpSocketPtr& s, uint16 cmd, const MsgBufPtr& buf) {
        m_ioCallback->onNewMsg(s, cmd, buf);
    }

protected:
    boost::asio::io_service& m_ios;  // io_service 外部构造ios 内部使用引用
    IoCallback* m_ioCallback;        // 逻辑回调
};

NS_BOOST_NETWORK_END

#endif

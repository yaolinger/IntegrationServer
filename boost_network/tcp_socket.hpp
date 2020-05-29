#ifndef BOOST_NETWORK_TCP_SOCKET
#define BOOST_NETWORK_TCP_SOCKET

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service_strand.hpp>

#include "buffer.hpp"
#include "io_interface.hpp"
#include "utils/macros.hpp"
#include "utils/noncopyable.hpp"

NS_BOOST_NETWORK_BEGIN

class TcpSocket : public std::enable_shared_from_this<TcpSocket>, private utils::Noncopyable {
public:
   explicit TcpSocket( boost::asio::io_service & ios, boost::asio::ip::tcp::socket socket, const boost::asio::ip::tcp::endpoint& addr, IoInterface* pIoInterface, uint32 index = 0);

   ~TcpSocket();
public:
    // 读数据
	void read();

    // 写数据(写入缓存区)
    // @param cmd: 协议号 buf:数据 isCompress:是否缩
	void write(uint16 cmd, const MsgBufPtr& buf, bool isCompress = false);

    // 主动关闭socket
    void close();

private:
    // 发起异步读
    void doRead(const TcpSocketPtr& self);

    // 发起异步写
    void doWrite(const TcpSocketPtr& self);

	// 关闭socket
    void doClose(const TcpSocketPtr& self);

    // 读取错误
    void onReadError(const TcpSocketPtr& self, const boost::system::error_code& ec);

    // 写入错误
    void onWriteError(const TcpSocketPtr& self, const boost::system::error_code& ec);

    // 解析数据包
    bool readParse(const TcpSocketPtr& self, uint32 recvLength);

    // 是否压缩数据
    // @param isCompressed:是否已经压缩 isCompress:是否压缩 msgSize:数据长度
    bool isNeedCompress(bool isCompressed, bool isCompress, uint32 msgSize);

    // 获取空闲可写缓存区索引
    uint8 getIdleIndex(uint8 index) { return index ^= 1; }
public:
    int32 getSocketId() { return m_socketId; }

    uint32 getMsgIndex() { return m_msgIndex; }

    uint32 getPort() { return m_port; }

    const std::string& getIp() { return m_ip; }

    IoInterface* getIoInterface() { return m_ioInterface; }

    TcpSocketPtr getSharedPtr() { return shared_from_this(); }

    boost::asio::ip::tcp::socket& getSocket() { return m_socket; }

private:
    boost::asio::io_service::strand m_strand;  // boost 串行队列
    boost::asio::ip::tcp::socket m_socket;     // boost socket

private:
    DynamicBuffer m_recvBuffer;      // 已接收数据缓存区
    DynamicBuffer m_sendBuffer[2];   // 待发送数据缓存区
    uint8 m_idleIndex = 0;           // 可写buf闲置索引
    bool m_asyncWriting = false;     // 异步写入中
	bool m_notifyCallback = true;    // 回调标识

    int32 m_socketId = -1;        // socket id
    uint32 m_msgIndex = 0;        // 消息计数
    uint32 m_port = 0;            // socket port
    std::string m_ip = "";        // socket ip

    IoInterface* m_ioInterface = NULL;      // io对象
};

typedef std::shared_ptr<TcpSocket> TcpSocketPtr;

NS_BOOST_NETWORK_END

#endif

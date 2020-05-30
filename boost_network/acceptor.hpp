#ifndef BOOST_NETWORK_ACCEPTOR_HPP
#define BOOST_NETWORK_ACCEPTOR_HPP

#include <string>
#include <vector>

#include <boost/asio/io_service_strand.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "io_interface.hpp"
#include "utils/macros.hpp"
#include "utils/noncopyable.hpp"

NS_BOOST_NETWORK_BEGIN

class Acceptor : public IoInterface
               , public std::enable_shared_from_this<Acceptor>
               , public utils::Noncopyable {
public:
    explicit Acceptor(boost::asio::io_service& ios, const std::string& listenIp, uint16 port, IoCallback* pIoCallback);

public:
    void onReadError(const TcpSocketPtr& s, const boost::system::error_code& ec);

    void onWriteError(const TcpSocketPtr& s, const boost::system::error_code& ec);

	void onActiveSocketClose(const TcpSocketPtr& s);

    void onPassiveSocketClose(const TcpSocketPtr& s);

public:
    // 开始监听
    bool listen();

    // 停止accept
    void stop();

    // 清空所有链接
    void clear();

private:
    // 发起异步处理链接
    void accept();

private:
    boost::asio::io_service::strand m_strand;      // 串行队列
    boost::asio::ip::tcp::acceptor m_acceptor;     // boost accept对象
    boost::asio::ip::tcp::socket m_newSocket;      // 新连接
    boost::asio::ip::tcp::endpoint m_newEndpoint;  // 新连接地址数据

    std::string m_listenIp;  // 监听ip
    uint16 m_listenPort;     // 监听端口

    std::vector<TcpSocketPtr> m_socketVec;  // socket列表
};

typedef std::shared_ptr<Acceptor> AcceptorPtr;

NS_BOOST_NETWORK_END
#endif

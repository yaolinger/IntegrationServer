#include "tcp_socket.hpp"

#include <functional>

#include <boost/asio/buffer.hpp>

#include "utils/log.hpp"
#include "utils/scope_guard.hpp"

NS_BOOST_NETWORK_BEGIN
USING_NS_UTILS;

TcpSocket::TcpSocket(boost::asio::io_service & ios, boost::asio::ip::tcp::socket socket, const boost::asio::ip::tcp::endpoint& addr, IoInterface* pIoInterface, uint32 index)
        : m_strand(ios)
        , m_socket(std::move(socket))
        , m_idleIndex(0)
        , m_asyncWriting(false)
		, m_notifyCallback(true)
        , m_socketId(m_socket.native_handle())
        , m_msgIndex(index)
        , m_port(addr.port())
        , m_ip(addr.address().to_string())
        , m_ioInterface(pIoInterface)
{
        // 关闭nagle算法
        m_socket.set_option(boost::asio::ip::tcp::no_delay(true));
}

TcpSocket::~TcpSocket() {
    if (m_socket.is_open()) {
        boost::system::error_code ec;
        m_socket.close(ec);
        CHECK_ERROR_LOG(ec, "Close socket[%d] was error[%s]", m_socketId, ec.message().c_str());
    }
}

void TcpSocket::read() {
    auto self(shared_from_this());
    doRead(self);
}

void TcpSocket::write(uint16 cmd, const MsgBufPtr& buf, bool isCompress) {
    auto self(shared_from_this());
    m_strand.post([this, self, cmd, buf, isCompress]() {
                bool needCompress = isNeedCompress(buf->isCompress(), isCompress, buf->size());
                if (needCompress) {
                    // TODO: 压缩处理

                } else {
                    if (buf->size() > PACKET_BODY_MAX_SIZE) {
                        log_error("Socket[%u] cmd[%u] buf size[%u] > PACKET_BODY_MAX_SIZE[%u] isCompress[%d]", m_socketId, cmd, buf->size(), PACKET_BODY_MAX_SIZE, buf->isCompress());
                        return;
                    }

                    uint32 packetHeadSize = PACKET_HEAD_SIZE;
                    uint32 msgAllSize = packetHeadSize + buf->size();
                    // 尝试调整缓存区大小
                    m_sendBuffer[m_idleIndex].reserve(msgAllSize);
                    if (m_sendBuffer[m_idleIndex].getWriteSize() < msgAllSize) {
                        log_error("Socket[%u] cmd[%u] write size[%u] < msg all size[%u]", m_socketId, cmd, m_sendBuffer[m_idleIndex].getWriteSize(), msgAllSize);
                        return;
                    }
                    // 填充头包数据
                    MsgHead* pHead = (MsgHead*)m_sendBuffer[m_idleIndex].getWritePos();
                    pHead->cmd = cmd;
                    pHead->index = self->m_msgIndex;
                    pHead->len = buf->size();
                    pHead->flag = 0;
                    if (buf->isCompress()) {
                        pHead->flag = PACKET_FLAG_ZIP;
                    }
                    // 填充包体数据
                    std::memcpy(&m_sendBuffer[m_idleIndex].getWritePos()[packetHeadSize], buf->data(), buf->size());
                    m_sendBuffer[m_idleIndex].addWirteLen(packetHeadSize + buf->size());
                }

                doWrite(self);
            });
}

void TcpSocket::close() {
   auto self(shared_from_this());
   m_strand.post([this, self]() {
                // TODO: 是否处理未发送成功数据包

				m_notifyCallback = false;
				doClose(self);
           });
}

void TcpSocket::doRead(const TcpSocketPtr& self) {
    m_socket.async_read_some(boost::asio::buffer(m_recvBuffer.getWritePos(), m_recvBuffer.getWriteSize()),
            m_strand.wrap([this, self](boost::system::error_code ec, std::size_t recvLength) {
                    do {
                        if (ec) {
                            onReadError(self, ec);
                            break;
                        }

                        if (false == readParse(self, recvLength)) {
                            onReadError(self, boost::system::errc::make_error_code(boost::system::errc::bad_message));
                            break;
                        }

                        doRead(self);
                    } while(0);
                }));
}

void TcpSocket::doWrite(const TcpSocketPtr& self) {
    // boost::asio::async_write
    // 异步写入数据只可以发起一次
    if (m_asyncWriting) {
        return;
    }

    // 无数据可写
    if (!m_sendBuffer[m_idleIndex].isReadOver()) {
        return;
    }

    // 切换空闲发送缓存区
    m_asyncWriting = true;
    uint8 asyncWriteIndex = m_idleIndex;
    m_idleIndex = getIdleIndex(m_idleIndex);

    // 消息异步发送成功后触发回调
    m_socket.async_send(boost::asio::buffer(m_sendBuffer[asyncWriteIndex].getReadPos(), m_sendBuffer[asyncWriteIndex].getReadSize()),
                m_strand.wrap([this, self](const boost::system::error_code& ec, std::size_t sendLength) {
                        if (ec) {
                            onWriteError(self, ec);
                            return;
                        }

                        // 调整当前发送缓存区
                        uint8 asyncWriteIndex = getIdleIndex(m_idleIndex);
                        m_sendBuffer[asyncWriteIndex].addReadLen(m_sendBuffer[asyncWriteIndex].getReadSize());
                        m_sendBuffer[asyncWriteIndex].refitBuffer();
                        m_asyncWriting = false;

                        doWrite(self);
                    }));
}

void TcpSocket::doClose(const TcpSocketPtr& self) {
    if (m_socket.is_open()) {
        log_debug("Close socket[%d] addr[%s:%u]", m_socketId, getIp().c_str(), getPort());
        boost::system::error_code ec;
        m_socket.cancel();
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        CHECK_ERROR_LOG(ec, "Shutdown socket[%d] was error[%s]", m_socketId, ec.message().c_str());
		m_socket.close(ec);
        CHECK_ERROR_LOG(ec, "Close socket[%d] was error[%s]", m_socketId, ec.message().c_str());
        // 触发逻辑回调
		if (m_notifyCallback) {
			m_ioInterface->onPassiveSocketClose(self);
		}
    }
}

void TcpSocket::onReadError(const TcpSocketPtr& self, const boost::system::error_code& ec) {
    if (ec == boost::asio::error::operation_aborted) {
        return;
    }
    m_ioInterface->onReadError(self, ec);
    doClose(self);
}

void TcpSocket::onWriteError(const TcpSocketPtr& self, const boost::system::error_code& ec) {
    if (ec == boost::asio::error::operation_aborted) {
        return;
    }
    m_ioInterface->onWriteError(self, ec);
    doClose(self);
}


bool TcpSocket::readParse(const TcpSocketPtr& self, uint32 recvLength) {
    m_recvBuffer.addWirteLen(recvLength);

    uint32 recvCount = 0;
    auto guard = NewScopeGuard([this, &recvCount]() {
                if (recvCount > 0) {
                    m_recvBuffer.refitBuffer();
                }
            });

    // 解析数据包
    do {
        // 检测缓存区长度
        if (m_recvBuffer.getReadSize() < PACKET_HEAD_SIZE) {
            break;
        }

        MsgHead* pHead = (MsgHead*)m_recvBuffer.getReadPos();
        uint32 msgSize = PACKET_HEAD_SIZE + pHead->len;
        if (msgSize > PACKET_MAX_SIZE) {
            log_error("Socket[%u] recv packet size[%u] > PACKET_MAX_SIZE[%u].", m_socketId, msgSize, PACKET_MAX_SIZE);
            return false;
        }
        if (msgSize > m_recvBuffer.getReadSize()) {
            break;
        }

        // 构建消息对象
        MsgBufPtr pBuff = std::make_shared<MsgBuf>(&(m_recvBuffer.getReadPos()[PACKET_HEAD_SIZE]), pHead->len, PACKET_FLAG_ZIP == (pHead->flag & PACKET_FLAG_ZIP));

        // 首包索引处理
        if (pHead->index > 0 && self->m_msgIndex == 0) {
            self->m_msgIndex = pHead->index;
        }
        // msgIndex = 0 服务内部通讯,不做索引检测
        // msgIndex > 0 计数检测
        if (self->m_msgIndex == 0) {
            m_ioInterface->onNewMsg(self, pHead->cmd, pBuff);
        } else if (pHead->index == self->m_msgIndex) {
            m_ioInterface->onNewMsg(self, pHead->cmd, pBuff);
            self->m_msgIndex += 1;
            if (self->m_msgIndex == 0xFF) {
                self->m_msgIndex = 1;
            }
        } else {
            log_error("Socket[%u] cmd[%u] client msg index[%u] != server msg index[%u].", self->m_socketId, pHead->cmd, pHead->index, self->m_msgIndex);
            return false;
        }

        // 增加已读数据
        m_recvBuffer.addReadLen(msgSize);
        recvCount++;
    } while(true);

    return true;
}

bool TcpSocket::isNeedCompress(bool isCompressed, bool isCompress, uint32 msgSize) {
    if (isCompressed) {
        return false;
    }

    if (msgSize > PACKET_BODY_MAX_SIZE) {
        return true;
    }

    if (msgSize >= PACKET_COMPRESS_MIN_SIZE && isCompress) {
        return true;
    }

    return false;
}

NS_BOOST_NETWORK_END

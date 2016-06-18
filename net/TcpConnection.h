#ifndef NET_TCPCONNECTION_H
#define NET_TCPCONNECTION_H

#include <net/InetAddress.h>
#include <net/Callbacks.h>
#include <net/Buffer.h>

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

namespace base {
namespace net {

class Channel;
class EventLoop;
class Socket;

class TcpConnection
  : boost::noncopyable,
    public boost::enable_shared_from_this<TcpConnection>
{
  public:
    TcpConnection(
      EventLoop* loop,
      const string& name,
      int sockfd,
      const InetAddress& localAddr,
      const InetAddress& peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const { return loop_; }
    const string& name() const { return name_; }
    const InetAddress& localAddress() const { return localAddr_; }
    const InetAddress& peerAddress() const { return peerAddr_; }

    bool connected() const { return state_ == kConnected; }
    bool disconnected() const { return state_ == kDisconnected; }

    void send(const void* message, int len);
    void send(const string& message);
    void send(Buffer* message);

    void shutdown();

    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }
    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb) { highWaterMarkCallback_ = cb; }
    void setCloseCallback(const CloseCallback& cb) { closeCallback_ = cb; }

    Buffer* inputBuffer() { return &inputBuffer_; }
    Buffer* outputBuffer() { return &outputBuffer_; }

    void connectEstablished();
    void connectDestroyed();

  private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const string& message);
    void sendInLoop(const void* message, size_t len);
    void shutdownInLoop();

    void setState(StateE s) { state_ = s; }

    EventLoop* loop_;
    const string name_;
    StateE state_;
    boost::scoped_ptr<Socket> socket_;
    boost::scoped_ptr<Channel> channel_;
    const InetAddress localAddr_;
    const InetAddress peerAddr_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    HighWaterMarkCallback highWaterMarkCallback_;
    CloseCallback closeCallback_;
    size_t hightWaterMark_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
};

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

}
}

#endif // NET_TCPCONNECTION_H

#include <net/TcpConnection.h>
#include <net/Socket.h>
#include <net/SocketsOps.h>
#include <net/Channel.h>
#include <net/EventLoop.h>
#include <base/Logging.h>
#include <boost/bind.hpp>
#include <assert.h>

using namespace base;
using namespace base::net;

void base::net::defaultConnectionCallback(const TcpConnectionPtr& conn) {
  LOG_INFO << "a new connection";
}

void base::net::defaultMessageCallback(
  const TcpConnectionPtr&,
  char* buf,
  Timestamp)
{
  LOG_INFO << buf;
}

TcpConnection::TcpConnection(
  EventLoop* loop,
  const string& nameArg,
  int sockfd,
  const InetAddress& localAddr,
  const InetAddress& peerAddr
)
  : loop_(loop),
    name_(nameArg),
    state_(kConnecting),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr)
{
  channel_->setReadCallback(boost::bind(&TcpConnection::handleRead, this, _1));
  channel_->setWriteCallback(boost::bind(&TcpConnection::handleWrite, this));
  channel_->setCloseCallback(boost::bind(&TcpConnection::handleClose, this));
  channel_->setErrorCallback(boost::bind(&TcpConnection::handleError, this));
  socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
  assert(state_ == kDisconnected);
}

void TcpConnection::connectEstablished() {
  loop_->assertInLoopThread();
  assert(state_ == kConnecting);
  setState(kConnected);
  channel_->tie(shared_from_this());
  channel_->enableReading();
  connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
  loop_->assertInLoopThread();
  if (state_ == kConnected) {
    setState(kDisconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());
  }
  channel_->remove();
}

void TcpConnection::handleRead(Timestamp receiveTime) {
  loop_->assertInLoopThread();
  char buf[1024];
  ssize_t n = sockets::read(channel_->fd(), buf, sizeof buf);
  if (n > 0) {
    messageCallback_(shared_from_this(), buf, receiveTime);
  } else if (n == 0) {
    handleClose();
  } else {
    handleError();
  }
}

void TcpConnection::handleWrite() {
  loop_->assertInLoopThread();
  if (channel_->isWriting()) {
    char buf[] = "a connection is writing";
    ssize_t n = sockets::write(channel_->fd(), buf, sizeof buf);
    if (n < 0) {
      LOG_ERROR << "TcpConnection::handleWrite";
    }
  }
}

void TcpConnection::handleClose() {
  loop_->assertInLoopThread();
  assert(state_ == kConnected || state_ == kDisconnecting);
  setState(kDisconnected);
  channel_->disableAll();

  TcpConnectionPtr guardThis(shared_from_this());
  connectionCallback_(guardThis);
  closeCallback_(guardThis);
}

void TcpConnection::handleError() {
  LOG_ERROR << "TcpConnection::handleError";
}

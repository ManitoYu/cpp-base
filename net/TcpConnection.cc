#include <net/TcpConnection.h>
#include <net/Socket.h>
#include <net/SocketsOps.h>
#include <net/Channel.h>
#include <net/EventLoop.h>
#include <base/Logging.h>

#include <boost/bind.hpp>

#include <assert.h>
#include <errno.h>

using namespace base;
using namespace base::net;

void base::net::defaultConnectionCallback(const TcpConnectionPtr& conn) {
  LOG_INFO << "a new connection";
}

void base::net::defaultMessageCallback(
  const TcpConnectionPtr&,
  Buffer* buffer,
  Timestamp)
{
  // LOG_INFO << buffer;
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
    peerAddr_(peerAddr),
    hightWaterMark_(64 * 1024 * 1024)
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

void TcpConnection::send(Buffer* buf) {
  if (state_ == kConnected) {
    if (loop_->isInLoopThread()) {
      sendInLoop(buf->peek(), buf->readableBytes());
      buf->retrieveAll();
    } else {
      loop_->runInLoop(
        boost::bind(&TcpConnection::sendInLoop, this, buf->retrieveAllAsString()));
    }
  }
}

void TcpConnection::sendInLoop(const string& message) {
  sendInLoop(message.c_str(), message.length());
}

void TcpConnection::sendInLoop(const void* data, size_t len) {
  loop_->assertInLoopThread();
  ssize_t nwrote = 0;
  size_t remaining = len;
  if (state_ == kDisconnected) {
    LOG_WARN << "disconnected";
    return;
  }

  if (! channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
    nwrote = sockets::write(channel_->fd(), data, len);
    if (nwrote >= 0) {
      remaining = len - nwrote;
      if (remaining == 0 && writeCompleteCallback_) {
        loop_->queueInLoop(boost::bind(writeCompleteCallback_, shared_from_this()));
      }
    } else {
      nwrote = 0;
      LOG_ERROR << "TcpConnection::sendInLoop";
    }
  }

  assert(remaining <= len);
  if (remaining > 0) {
    size_t oldLen = outputBuffer_.readableBytes();
    if (oldLen + remaining >= hightWaterMark_
      && oldLen < hightWaterMark_
      && highWaterMarkCallback_
    ) {
      loop_->queueInLoop(boost::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
    }
    outputBuffer_.append(static_cast<const char*>(data) + nwrote, remaining);
    if (! channel_->isWriting()) {
      channel_->enableWriting();
    }
  }
}

void TcpConnection::shutdown() {
  if (state_ == kConnected) {
    setState(kDisconnecting);
    loop_->runInLoop(boost::bind(&TcpConnection::shutdownInLoop, this));
  }
}

void TcpConnection::shutdownInLoop() {
  loop_->assertInLoopThread();
  if (! channel_->isWriting()) {
    socket_->shutdownWrite();
  }
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
  int savedErrno = 0;
  ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
  if (n > 0) {
    messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
  } else if (n == 0) {
    handleClose();
  } else {
    errno = savedErrno;
    handleError();
  }
}

void TcpConnection::handleWrite() {
  loop_->assertInLoopThread();
  if (channel_->isWriting()) {
    char buf[] = "a connection is writing";
    ssize_t n = sockets::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
    if (n < 0) {
      LOG_ERROR << "TcpConnection::handleWrite";
    } else {
      outputBuffer_.retrieve(n);
      if (outputBuffer_.readableBytes() == 0) {
        channel_->disableWriting();
        if (writeCompleteCallback_) {
          loop_->queueInLoop(boost::bind(writeCompleteCallback_, shared_from_this()));
        }
        if (state_ == kDisconnecting) {
          shutdownInLoop();
        }
      }
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

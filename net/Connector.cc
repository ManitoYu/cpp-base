#include <net/Connector.h>
#include <net/EventLoop.h>
#include <net/Channel.h>
#include <net/SocketsOps.h>
#include <base/Logging.h>

#include <boost/bind.hpp>

#include <errno.h>

using namespace base;
using namespace base::net;

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
  : loop_(loop),
    serverAddr_(serverAddr),
    connect_(false),
    state_(kDisconnected),
    retryDelayMs_(kInitRetryDelayMs)
{
}

Connector::~Connector() {
  assert(! channel_);
}

void Connector::start() {
  connect_ = true;
  loop_->runInLoop(boost::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop() {
  loop_->assertInLoopThread();
  assert(state_ == kDisconnected);
  if (connect_) connect();
}

void Connector::stop() {
  connect_ = false;
  loop_->runInLoop(boost::bind(&Connector::stopInLoop, this));
}

void Connector::stopInLoop() {
  loop_->assertInLoopThread();
  if (state_ == kConnecting) {
    setState(kDisconnected);
    int sockfd = removeAndResetChannel();
    retry(sockfd);
  }
}

void Connector::connect() {
  int sockfd = sockets::createNonblockingOrDie(serverAddr_.family());
  int ret = sockets::connect(sockfd, serverAddr_.getSockAddr());
  int savedErrno = ret == 0 ? 0 : errno;
  switch (savedErrno) {
    case 0:
    case EINPROGRESS: // 115
    case EINTR:
    case EISCONN:
      connecting(sockfd);
      break;

    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
      retry(sockfd);
      break;

    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
      LOG_ERROR << "connect error in Connector::startInLoop " << savedErrno;
      sockets::close(sockfd);
      break;

    default:
      LOG_ERROR << "Unexpected error in Connector::startInLoop " << savedErrno;
      sockets::close(sockfd);
      break;
  }
}

void Connector::restart() {
  loop_->assertInLoopThread();
  setState(kDisconnected);
  retryDelayMs_ = kInitRetryDelayMs;
  start();
}

void Connector::connecting(int sockfd) {
  setState(kConnecting);
  assert(! channel_);
  channel_.reset(new Channel(loop_, sockfd));
  channel_->setWriteCallback(
    boost::bind(&Connector::handleWrite, this));
  channel_->setErrorCallback(
    boost::bind(&Connector::handleError, this));
  channel_->enableWriting();
}

int Connector::removeAndResetChannel() {
  channel_->disableAll();
  channel_->remove();
  int sockfd = channel_->fd();
  loop_->queueInLoop(boost::bind(&Connector::resetChannel, this));
  return sockfd;
}

void Connector::resetChannel() {
  channel_.reset();
}

void Connector::handleWrite() {
  if (state_ == kConnecting) {
    int sockfd = removeAndResetChannel();
    // FIXME error
    setState(kConnected);
    if (connect_) {
      newConnectionCallback_(sockfd);
    } else {
      sockets::close(sockfd);
    }
  }
}

void Connector::handleError() {
  LOG_ERROR << "Connector::handleError";
  if (state_ == kConnecting) {
    int sockfd = removeAndResetChannel();
    // FIXME error
    retry(sockfd);
  }
}

void Connector::retry(int sockfd) {
  sockets::close(sockfd);
  setState(kDisconnected);
  if (connect_) {
    LOG_INFO << "Connector::retry";
    loop_->runAfter(retryDelayMs_ / 1000.0,
      boost::bind(&Connector::startInLoop, shared_from_this()));
    retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
  }
}

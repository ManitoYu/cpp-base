#include <net/Socket.h>
#include <net/InetAddress.h>
#include <net/SocketsOps.h>
#include <strings.h>
#include <base/Logging.h>
#include <netinet/in.h> // IPPROTO_TCP
#include <netinet/tcp.h> // TCP_NODELAY

using namespace base;
using namespace base::net;

Socket::~Socket() {
  sockets::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& addr) {
  sockets::bindOrDie(sockfd_, addr.getSockAddr());
}

void Socket::listen() {
  sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peeraddr) {
  struct sockaddr_in addr;
  bzero(&addr, sizeof addr);
  int connfd = sockets::accept(sockfd_, &addr);
  if (connfd >= 0) {
    peeraddr->setSockAddrInet(addr);
  }
  return connfd;
}

void Socket::shundownWrite() {
  sockets::shutdownWrite(sockfd_);
}

void Socket::setTcpNoDelay(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
    &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReuseAddr(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
    &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReusePort(bool on) {
  int optval = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
    &optval, static_cast<socklen_t>(sizeof optval));
  if (on && ret < 0) {
    LOG_ERROR << "SO_REUSEPORT failed";
  }
}

void Socket::setKeepAlive(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
    &optval, static_cast<socklen_t>(sizeof optval));
}

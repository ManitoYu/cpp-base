#include <net/SocketsOps.h>
#include <base/Logging.h>
#include <net/Endian.h>
#include <base/Types.h>
#include <stdio.h> // snprintf
#include <sys/socket.h> // accept ...
#include <strings.h> // bzero
#include <unistd.h> // close

using namespace base;
using namespace base::net;

const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in* addr) {
  return static_cast<const struct sockaddr*>(static_cast<const void*>(addr));
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in* addr) {
  return static_cast<struct sockaddr*>(static_cast<void*>(addr));
}

const struct sockaddr_in* sockets::sockaddr_in_cast(const struct sockaddr* addr) {
  return static_cast<const struct sockaddr_in*>(static_cast<const void*>(addr));
}

int sockets::createNonblockingOrDie(sa_family_t family) {
  int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
  if (sockfd < 0) {
    LOG_FATAL << "sockets::createNonblockingOrDie";
  }
  return sockfd;
}

void sockets::bindOrDie(int sockfd, const struct sockaddr* addr) {
  int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in)));
  if (ret < 0) {
    LOG_FATAL << "sockets::bindOrDie";
  }
}

void sockets::listenOrDie(int sockfd) {
  int ret = ::listen(sockfd, SOMAXCONN);
  if (ret < 0) {
    LOG_FATAL << "sockets::listenOrDie";
  }
}

int sockets::accept(int sockfd, struct sockaddr_in* addr) {
  socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
  int connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
  if (connfd < 0) {
    LOG_FATAL << "sockets::accept";
  }
  return connfd;
}

int sockets::connect(int sockfd, const struct sockaddr* addr) {
  return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in)));
}

ssize_t sockets::read(int sockfd, void* buf, size_t count) {
  return ::read(sockfd, buf, count);
}

ssize_t sockets::write(int sockfd, void* buf, size_t count) {
  return ::write(sockfd, buf, count);
}

void sockets::close(int sockfd) {
  if (::close(sockfd) < 0) {
    LOG_ERROR << "sockets::close";
  }
}

void sockets::shutdownWrite(int sockfd) {
  if (::shutdown(sockfd, SHUT_WR) < 0) {
    LOG_ERROR << "sockets::shutdownWrite";
  }
}

void sockets::toIpPort(char* buf, size_t size, const struct sockaddr* addr) {
  toIp(buf, size, addr);
  size_t end = ::strlen(buf);
  const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
  uint16_t port = sockets::networkToHost16(addr4->sin_port);
  snprintf(buf + end, size - end, ":%u", port);
}

void sockets::toIp(char* buf, size_t size, const struct sockaddr* addr) {
  assert(size >= INET_ADDRSTRLEN);
  const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
  ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
}

void sockets::fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr) {
  addr->sin_family = AF_INET;
  addr->sin_port = hostToNetwork16(port);
  if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
    LOG_ERROR << "sockets::fromIpPort";
  }
}

struct sockaddr_in sockets::getLocalAddr(int sockfd) {
  struct sockaddr_in localaddr;
  bzero(&localaddr, sizeof localaddr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
  if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0) {
    LOG_ERROR << "sockets::getLocalAddr";
  }
  return localaddr;
}

struct sockaddr_in sockets::getPeerAddr(int sockfd) {
  struct sockaddr_in peeraddr;
  bzero(&peeraddr, sizeof peeraddr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
  if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0) {
    LOG_ERROR << "sockets::getPeerAddr";
  }
  return peeraddr;
}

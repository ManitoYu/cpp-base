#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include <boost/noncopyable.hpp>

namespace base {
namespace net {

class InetAddress;

class Socket : boost::noncopyable {
  public:
    explicit Socket(int sockfd) : sockfd_(sockfd) {
    }

    ~Socket();

    int fd() const { return sockfd_; }

    void bindAddress(const InetAddress& localaddr);
    void listen();
    int accept(InetAddress* peeraddr);

    void shundownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

  private:
    const int sockfd_;
};

}
}

#endif // NET_SOCKET_H

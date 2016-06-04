#ifndef NET_INETADDRESS_H
#define NET_INETADDRESS_H

#include <base/Types.h>
#include <netinet/in.h> // sa_family_t

namespace base {
namespace net {
namespace sockets {

const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);

}

class InetAddress {
  public:
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false);
    InetAddress(string ip, uint16_t port);

    sa_family_t family() const { return addr_.sin_family; }
    string toIp() const;
    string toIpPort() const;
    uint16_t toPort() const;

    const struct sockaddr* getSockAddr() const { return sockets::sockaddr_cast(&addr_); }

    uint32_t ipNetEndian() const;
    uint16_t portNetEndian() const { return addr_.sin_port; }

  private:
    struct sockaddr_in addr_;
};

}
}


#endif // NET_INETADDRESS_H

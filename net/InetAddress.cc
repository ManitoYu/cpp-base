#include <net/InetAddress.h>
#include <strings.h>
#include <net/Endian.h>
#include <net/SocketsOps.h>

static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;

using namespace base;
using namespace base::net;

InetAddress::InetAddress(uint16_t port, bool loopbackOnly) {
  bzero(&addr_, sizeof addr_);
  addr_.sin_family = AF_INET;
  in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
  addr_.sin_addr.s_addr = sockets::hostToNetwork32(ip);
  addr_.sin_port = sockets::hostToNetwork16(port);
}

InetAddress::InetAddress(string ip, uint16_t port) {
  bzero(&addr_, sizeof addr_);
  sockets::fromIpPort(ip.c_str(), port, &addr_);
}

string InetAddress::toIpPort() const {
  char buf[64] = "";
  sockets::toIpPort(buf, sizeof buf, getSockAddr());
  return buf;
}

string InetAddress::toIp() const {
  char buf[64] = "";
  sockets::toIp(buf, sizeof buf, getSockAddr());
  return buf;
}

uint16_t InetAddress::toPort() const {
  return sockets::networkToHost16(portNetEndian());
}

uint32_t InetAddress::ipNetEndian() const {
  assert(family() == AF_INET);
  return addr_.sin_addr.s_addr;
}

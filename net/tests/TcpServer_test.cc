#include <net/TcpServer.h>
#include <net/EventLoop.h>
#include <net/InetAddress.h>

using namespace base;
using namespace base::net;

int main() {
  EventLoop loop;
  InetAddress addr(3000, true);

  TcpServer server(&loop, addr, "TestServer");
  server.start();

  loop.loop();

  return 0;
}

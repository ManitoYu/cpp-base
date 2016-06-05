#include <net/Acceptor.h>
#include <net/EventLoop.h>
#include <net/InetAddress.h>
#include <base/Logging.h>

using namespace base;
using namespace base::net;

int main() {

  EventLoop loop;
  InetAddress addr(8000);
  Acceptor acceptor(&loop, addr);
  acceptor.setNewConnectionCallback([](int sockfd, const InetAddress& addr) {
    LOG_INFO << "new connection " << addr.toIpPort();
    char buf[] = "welcome to my server";
    ::write(sockfd, buf, sizeof buf);
    ::close(sockfd);
  });
  acceptor.listen();

  loop.loop();

  return 0;
}

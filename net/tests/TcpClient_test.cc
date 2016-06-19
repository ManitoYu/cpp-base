#include <base/Logging.h>
#include <net/TcpClient.h>
#include <net/EventLoop.h>
#include <net/Channel.h>

using namespace base;
using namespace base::net;

int main() {
  EventLoop loop;
  InetAddress addr("127.0.0.1", 3000);

  TcpClient client(&loop, addr, "client");

  Channel channel(&loop, 0);
  channel.setReadCallback([&](Timestamp timestamp) {
    char buf[1024] = { 0 };
    ::read(0, buf, sizeof buf);
    buf[strlen(buf) - 1] = '\0';
    client.connection()->send(buf, strlen(buf));
  });
  channel.enableReading();

  client.setConnectionCallback([&](const TcpConnectionPtr& conn) {
    LOG_INFO << "connect successfully";
  });

  client.setMessageCallback([](const TcpConnectionPtr& conn, Buffer* buf, Timestamp timestamp) {
    LOG_INFO << "new message => " << buf->retrieveAllAsString();
  });

  client.connect();

  loop.loop();

  return 0;
}

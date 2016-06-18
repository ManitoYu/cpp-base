#include <net/TcpServer.h>
#include <net/EventLoop.h>
#include <net/InetAddress.h>
#include <base/Logging.h>

using namespace base;
using namespace base::net;

int main() {
  Logger::setLogLevel(Logger::TRACE);

  EventLoop loop;
  InetAddress addr(3000, true);

  TcpServer server(&loop, addr, "TestServer");
  server.setConnectionCallback([](const TcpConnectionPtr& conn) {
    LOG_INFO << "a new connection";
  });
  server.setMessageCallback([](const TcpConnectionPtr& conn, Buffer* buf, Timestamp timestamp) {
    // LOG_INFO << buf->retrieveAllAsString();
    conn->send(buf);
  });
  server.setWriteCompleteCallback([](const TcpConnectionPtr& conn) {

  });

  server.start();

  loop.loop();

  return 0;
}

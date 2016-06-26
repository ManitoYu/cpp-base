#include <net/http/HttpServer.h>
#include <net/http/HttpRequest.h>
#include <net/http/HttpResponse.h>
#include <net/EventLoop.h>
#include <base/Logging.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace base;
using namespace base::net;

int main() {

  EventLoop loop;
  HttpServer server(&loop, InetAddress(80), "test");

  server.setHttpCallback([](const HttpRequest& req, HttpResponse* res) {
    LOG_INFO << req.path();

    int fd = open("/tmp/index.html", O_RDONLY);

    char data[1024] = { 0 };
    ::read(fd, data, sizeof data);

    res->setStatusCode(HttpResponse::k200Ok);
    res->setStatusMessage("OK");
    res->setContentType("text/html");
    res->setBody(data);
  });
  server.start();

  loop.loop();

  return 0;
}

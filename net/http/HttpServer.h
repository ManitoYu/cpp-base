#ifndef NET_HTTP_HTTPSERVER_H
#define NET_HTTP_HTTPSERVER_H

#include <net/TcpServer.h>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace base {
namespace net {

class HttpRequest;
class HttpResponse;

class HttpServer : boost::noncopyable {
  public:
    typedef boost::function<void (const HttpRequest&, HttpResponse*)> HttpCallback;

    HttpServer(
      EventLoop* loop,
      const InetAddress& listenAddr,
      const string& name,
      TcpServer::Option option = TcpServer::kNoReusePort);

    ~HttpServer();

    EventLoop* getLoop() const { return server_.getLoop(); }

    void setHttpCallback(const HttpCallback& cb) {
      httpCallback_ = cb;
    }

    void start();

  private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime);
    void onRequest(const TcpConnectionPtr& conn, const HttpRequest& req);

    TcpServer server_;
    HttpCallback httpCallback_;
};

}
}

#endif // NET_HTTP_HTTPSERVER_H

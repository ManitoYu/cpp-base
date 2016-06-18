#ifndef NET_TCPSERVER_H
#define NET_TCPSERVER_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <base/Atomic.h>
#include <net/TcpConnection.h>
#include <map>

namespace base {
namespace net {

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer : boost::noncopyable {
  public:
    typedef boost::function<void (EventLoop*)> ThreadInitCallback;
    enum Option {
      kNoReusePort,
      kReusePort
    };

    TcpServer(
      EventLoop* loop,
      const InetAddress& listenAddr,
      const string& nameArg,
      Option option = kNoReusePort);
    ~TcpServer();

    const string& ipPort() const { return ipPort_; }
    const string& name() const { return name_; }
    EventLoop* getLoop() const { return loop_; }

    void setThreadNum(int numThreads);
    void setThreadInitCallback(const ThreadInitCallback& cb) { threadInitCallback_ = cb; }
    boost::shared_ptr<EventLoopThreadPool> threadPool() const { return threadPool_; }

    void start();

    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

  private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    typedef std::map<string, TcpConnectionPtr> ConnectionMap;

    EventLoop* loop_;
    const string ipPort_;
    const string name_;
    boost::scoped_ptr<Acceptor> acceptor_;
    boost::shared_ptr<EventLoopThreadPool> threadPool_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    ThreadInitCallback threadInitCallback_;
    AtomicInt32 started_;
    int nextConnId_;
    ConnectionMap connections_;
};

}
}

#endif // NET_TCPSERVER_H

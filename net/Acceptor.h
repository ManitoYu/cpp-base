#ifndef NET_ACCEPTOR_H
#define NET_ACCEPTOR_H

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <net/Channel.h>
#include <net/Socket.h>

namespace base {
namespace net {

class EventAddress;
class InetAddress;

class Acceptor : boost::noncopyable {
  public:
    typedef boost::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport = true);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb) { newConnectionCallback_ = cb; }

    bool listenning() const { return listenning_; }
    void listen();

  private:
    void handleRead();

    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
    int idleFd_;
};

}
}

#endif // NET_ACCEPTOR_H

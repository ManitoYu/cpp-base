#ifndef NET_POLLER_H
#define NET_POLLER_H

#include <boost/noncopyable.hpp>
#include <map>
#include <vector>
#include <net/EventLoop.h>

namespace base {
namespace net{

class Channel;

class Poller : boost::noncopyable {
  public:
    typedef std::vector<Channel*> ChannelList;

    Poller(EventLoop* loop);
    virtual ~Poller();

    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;

    void assertInLoopThread() const { ownerLoop_->assertInLoopThread(); }

  protected:
    typedef std::map<int, Channel*> ChannelMap;
    ChannelMap channels_;

  private:
    EventLoop* ownerLoop_; // poller所属的EventLoop
};

}
}

#endif

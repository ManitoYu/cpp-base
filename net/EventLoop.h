#ifndef NET_EVENTLOOP_H
#define NET_EVENTLOOP_H

#include <boost/noncopyable.hpp>
#include <base/CurrentThread.h>
#include <base/Types.h>
#include <boost/scoped_ptr.hpp>
#include <base/Timestamp.h>
#include <vector>

namespace base {
namespace net {

class Channel;
class Poller;

class EventLoop : boost::noncopyable {
  public:
    EventLoop();
    ~EventLoop();

    void loop();

    void assertInLoopThread() {
      if (! isInLoopThread()) abortNotInLoopThread();
    }
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

    void updateChannel(Channel* channel); // 添加或更新通道
    void removeChannel(Channel* channel); // 移除通道

  private:
    void abortNotInLoopThread();

    typedef std::vector<Channel*> ChannelList;

    bool looping_;
    bool quit_;
    bool eventHandling_; // 是否处于事件处理的状态
    const pid_t threadId_;
    Timestamp pollReturnTime_;
    boost::scoped_ptr<Poller> poller_;

    ChannelList activeChannels_; // poller返回的活动通道
    Channel* currentActiveChannel_; // 当前正在处理的活动通道
};

}
}

#endif // NET_EVENTLOOP_H

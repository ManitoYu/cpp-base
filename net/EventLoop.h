#ifndef NET_EVENTLOOP_H
#define NET_EVENTLOOP_H

#include <boost/noncopyable.hpp>
#include <base/CurrentThread.h>
#include <base/Types.h>
#include <boost/scoped_ptr.hpp>
#include <base/Timestamp.h>
#include <vector>
#include <net/Callbacks.h>
#include <net/TimerId.h>
#include <base/Mutex.h>

namespace base {
namespace net {

class Channel;
class Poller;
class TimerQueue;

class EventLoop : boost::noncopyable {
  public:
    typedef boost::function<void()> Functor;

    EventLoop();
    ~EventLoop();

    void loop();

    void quit();

    void runInLoop(const Functor& cb);
    void queueInLoop(const Functor& cb);

    TimerId runAt(const Timestamp& time, const TimerCallback& cb);
    TimerId runAfter(double delay, const TimerCallback& cb);
    TimerId runEvery(double interval, const TimerCallback& cb);

    void cancel(TimerId timerId);

    void assertInLoopThread() {
      if (! isInLoopThread()) abortNotInLoopThread();
    }
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

    void wakeup();
    void updateChannel(Channel* channel); // 添加或更新通道
    void removeChannel(Channel* channel); // 移除通道

  private:
    void abortNotInLoopThread();
    void handleRead();
    void doPendingFunctors();

    typedef std::vector<Channel*> ChannelList;

    bool looping_;
    bool quit_;
    bool eventHandling_; // 是否处于事件处理的状态
    bool callingPendingFunctors_;
    const pid_t threadId_;
    Timestamp pollReturnTime_;
    boost::scoped_ptr<Poller> poller_;
    boost::scoped_ptr<TimerQueue> timerQueue_;

    int wakeupfd_;
    boost::scoped_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_; // poller返回的活动通道
    Channel* currentActiveChannel_; // 当前正在处理的活动通道

    MutexLock mutex_;
    std::vector<Functor> pendingFunctors_;
};

}
}

#endif // NET_EVENTLOOP_H

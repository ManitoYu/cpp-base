#ifndef NET_TIMERQUEUE_H
#define NET_TIMERQUEUE_H

#include <set>
#include <vector>
#include <boost/noncopyable.hpp>
#include <base/Timestamp.h>
#include <net/Callbacks.h>
#include <net/Channel.h>

namespace base {
namespace net {

class EventLoop;
class Timer;
class TimerId;

class TimerQueue : boost::noncopyable {
  public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId addTimer(const TimerCallback& cb, Timestamp when, double interval);
    void cancel(TimerId timerId);

  private:
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);

    void handleRead();

    std::vector<Entry> getExpired(Timestamp now); // 返回超时的定时器列表
    void reset(const std::vector<Entry>& expired, Timestamp now);

    bool insert(Timer* timer);

    EventLoop* loop_;
    const int timerfd_;
    Channel timerfdChannel_;

    TimerList timers_;

    ActiveTimerSet activeTimers_;
    bool callingExpiredTimers_;
    ActiveTimerSet cancelingTimers_;
};


}
}

#endif // NET_TIMERQUEUE_H

#ifndef NET_TIMER_H
#define NET_TIMER_H

#include <boost/noncopyable.hpp>
#include <base/Timestamp.h>
#include <base/Atomic.h>
#include <net/Callbacks.h>

namespace base {
namespace net {

class Timer : boost::noncopyable {
  public:
    Timer(const TimerCallback& cb, Timestamp when, double interval)
      : callback_(cb),
        expiration_(when),
        interval_(interval),
        repeat_(interval > 0.0),
        sequence_(s_numCreated_.incrementAndGet())
    {
    }

    void run() const {
      callback_();
    }

    Timestamp expiration() const { return expiration_; }
    bool repeat() const { return repeat_; }
    int64_t sequence() const { return sequence_; }

    void restart(Timestamp now);

    static int64_t numCreated() { return s_numCreated_.get(); }

  private:
    const TimerCallback callback_;
    Timestamp expiration_; // 下一次超时时刻
    const double interval_; // 定时间隔
    const bool repeat_; // 是否重复
    const int64_t sequence_; // 定时器序号

    static AtomicInt64 s_numCreated_; // 已经创建的定时器数量
};

}
}

#endif // NET_TIMER_H

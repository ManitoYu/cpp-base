#include <net/TimerQueue.h>
#include <base/Logging.h>
#include <net/EventLoop.h>
#include <net/Timer.h>
#include <net/TimerId.h>
#include <boost/bind.hpp>
#include <sys/timerfd.h>

namespace base {
namespace net {
namespace detail {

int createTimerfd() {
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd < 0) LOG_FATAL << "failed in timerfd_create";
  return timerfd;
}

// 超时时刻与当前时刻时间差
struct timespec howMuchTimeFromNow(Timestamp when) {
  int64_t microSeconds = when.microSeconds() - Timestamp::now().microSeconds();
  if (microSeconds < 100) microSeconds = 100;
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(microSeconds / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>((microSeconds % Timestamp::kMicroSecondsPerSecond) * 1000);
  return ts;
}

void readTimerfd(int timerfd, Timestamp now) {
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
}

void resetTimerfd(int timerfd, Timestamp expiration) {
  struct itimerspec newValue;
  struct itimerspec oldValue;
  bzero(&newValue, sizeof newValue);
  bzero(&oldValue, sizeof oldValue);
  newValue.it_value = howMuchTimeFromNow(expiration);
  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
  if (ret) LOG_FATAL << "timerfd_settime()";
}

}
}
}

using namespace base;
using namespace base::net;
using namespace base::net::detail;

TimerQueue::TimerQueue(EventLoop* loop)
  : loop_(loop),
    timerfd_(createTimerfd()),
    timerfdChannel_(loop, timerfd_),
    timers_(),
    callingExpiredTimers_(false)
{
  timerfdChannel_.setReadCallback(boost::bind(&TimerQueue::handleRead, this));
  timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue() {
  timerfdChannel_.disableAll();
  timerfdChannel_.remove();
  ::close(timerfd_);
  for (TimerList::iterator it = timers_.begin();
    it != timers_.end(); it ++)
  {
    delete it->second;
  }
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval) {
  Timer* timer = new Timer(cb, when, interval);
  loop_->runInLoop(boost::bind(&TimerQueue::addTimerInLoop, this, timer));
  return TimerId(timer, timer->sequence());
}

void TimerQueue::cancel(TimerId timerId) {
  cancelInLoop(timerId);
}

void TimerQueue::addTimerInLoop(Timer* timer) {
  loop_->assertInLoopThread();
  bool earliestChanged = insert(timer);
  if (earliestChanged) resetTimerfd(timerfd_, timer->expiration());
}

void TimerQueue::cancelInLoop(TimerId timerId) {
  loop_->assertInLoopThread();
  assert(timers_.size() == activeTimers_.size());
  ActiveTimer timer(timerId.timer_, timerId.sequence_);
  ActiveTimerSet::iterator it = activeTimers_.find(timer);
  if (it != activeTimers_.end()) {
    size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
    assert(n == 1);
    delete it->first;
    activeTimers_.erase(it);
  } else if (callingExpiredTimers_) {
    cancelingTimers_.insert(timer);
  }
  assert(timers_.size() == activeTimers_.size());
}

void TimerQueue::handleRead() {
  loop_->assertInLoopThread();
  Timestamp now(Timestamp::now());
  readTimerfd(timerfd_, now); // 清除可读事件，避免一直触发

  std::vector<Entry> expired = getExpired(now);

  callingExpiredTimers_ = true;
  cancelingTimers_.clear();
  for (std::vector<Entry>::iterator it = expired.begin();
    it != expired.end(); it ++)
  {
    it->second->run(); // 定时器处理函数
  }
  callingExpiredTimers_ = false;

  // 若不为一次性定时器，还需重启
  reset(expired, now);
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now) {
  Timestamp nextExpire;

  for (std::vector<Entry>::const_iterator it = expired.begin();
    it != expired.end(); it ++)
  {
    ActiveTimer timer(it->second, it->second->sequence());
    // 若果是重复的定时器并且是未取消的定时器，则重启该定时器
    if (it->second->repeat()
      && cancelingTimers_.find(timer) == cancelingTimers_.end())
    {
      it->second->restart(now); // 重新计算下一个超时时刻
      insert(it->second);
    } else {
      delete it->second;
    }
  }

  if (! timers_.empty()) {
    nextExpire = timers_.begin()->second->expiration();
  }

  if (nextExpire.valid()) {
    resetTimerfd(timerfd_, nextExpire);
  }
}

// rvo return value optimization
std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
  assert(timers_.size() == activeTimers_.size());
  std::vector<Entry> expired;
  // 最大地址 UINTPTR_MAX 18446744073709551615UL
  Entry sentry(now, reinterpret_cast<Timer*>(18446744073709551615UL));
  TimerList::iterator end = timers_.lower_bound(sentry);
  assert(end == timers_.end() || now < end->first); // 闭开区间
  // 插入迭代器
  std::copy(timers_.begin(), end, back_inserter(expired));
  timers_.erase(timers_.begin(), end); // 移除到期定时器

  for (std::vector<Entry>::iterator it = expired.begin();
    it != expired.end(); it ++)
  {
    ActiveTimer timer(it->second, it->second->sequence());
    size_t n = activeTimers_.erase(timer);
    assert(n == 1);
  }

  assert(timers_.size() == activeTimers_.size());
  return expired;
}

bool TimerQueue::insert(Timer* timer) {
  loop_->assertInLoopThread();
  assert(timers_.size() == activeTimers_.size());
  bool earliestChanged = false;
  Timestamp when = timer->expiration();
  TimerList::iterator it = timers_.begin();
  if (it == timers_.end() || when < it->first) earliestChanged = true;
  {
    std::pair<TimerList::iterator, bool> result
      = timers_.insert(Entry(when, timer));
    assert(result.second);
  }
  {
    std::pair<ActiveTimerSet::iterator, bool> result
      = activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
    assert(result.second);
  }
  assert(timers_.size() == activeTimers_.size());
  return earliestChanged;
}

#include <net/EventLoop.h>
#include <base/Logging.h>
#include <poll.h>
#include <net/Poller.h>
#include <net/Channel.h>
#include <net/poller/PollPoller.h>
#include <net/poller/EPollPoller.h>
#include <net/TimerQueue.h>

using namespace base;
using namespace base::net;

namespace {
  __thread EventLoop* t_loopInThisThread = NULL; // 线程局部存储
}

EventLoop::EventLoop()
  : looping_(false),
    quit_(false),
    eventHandling_(false),
    threadId_(CurrentThread::tid()),
    poller_(new EPollPoller(this)),
    timerQueue_(new TimerQueue(this)),
    currentActiveChannel_(NULL)
{
  if (t_loopInThisThread) {
    // 如果当前线程已经创建了EventLoop对象，则终止进程
    LOG_FATAL << "Another EventLoop has been created in this thread " << threadId_;
  } else {
    t_loopInThisThread = this;
  }
}

EventLoop::~EventLoop() {
  t_loopInThisThread = NULL;
}

// 事件循环，该函数不能跨线程调用，只能在创建该对象的线程中调用
void EventLoop::loop() {
  assert(! looping_);
  assertInLoopThread();

  looping_ = true;
  quit_ = false;
  LOG_INFO << "EventLoop " << this << " start looping";

  while (! quit_) {
    activeChannels_.clear();
    pollReturnTime_ = poller_->poll(5000, &activeChannels_);
    // 事件处理
    eventHandling_ = true;
    for (ChannelList::iterator it = activeChannels_.begin();
      it != activeChannels_.end(); it ++)
    {
      currentActiveChannel_ = *it;
      currentActiveChannel_->handleEvent(pollReturnTime_);
    }
    currentActiveChannel_ = NULL;
    eventHandling_ = false;
  }

  LOG_INFO << "EventLoop " << this << " stop looping";
  looping_ = false;
}

void EventLoop::quit() {
  quit_ = true;
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb) {
  return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb) {
  Timestamp time(addTime(Timestamp::now(), delay));
  return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb) {
  Timestamp time(addTime(Timestamp::now(), interval));
  return timerQueue_->addTimer(cb, time, interval);
}

void EventLoop::cancel(TimerId timerId) {
  return timerQueue_->cancel(timerId);
}

void EventLoop::abortNotInLoopThread() {
  LOG_FATAL
    << "EventLoop::abortNotInLoopThread - EventLoop " << this
    << " was created in threadId_ = " << threadId_
    << ", current thread id = " << CurrentThread::tid();
}

void EventLoop::updateChannel(Channel* channel) {
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  poller_->removeChannel(channel);
}

#include <net/EventLoop.h>
#include <base/Logging.h>
#include <poll.h>
#include <net/Poller.h>
#include <net/Channel.h>
#include <net/poller/PollPoller.h>
#include <net/poller/EPollPoller.h>
#include <net/TimerQueue.h>
#include <sys/eventfd.h>
#include <boost/bind.hpp>

using namespace base;
using namespace base::net;

namespace {
  __thread EventLoop* t_loopInThisThread = NULL; // 线程局部存储

  int createEventFd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
      LOG_ERROR << "Failed in eventfd";
      abort();
    }
    return evtfd;
  }

}

EventLoop::EventLoop()
  : looping_(false),
    quit_(false),
    eventHandling_(false),
    callingPendingFunctors_(false),
    threadId_(CurrentThread::tid()),
    poller_(new EPollPoller(this)),
    timerQueue_(new TimerQueue(this)),
    wakeupfd_(createEventFd()),
    wakeupChannel_(new Channel(this, wakeupfd_)),
    currentActiveChannel_(NULL)
{
  if (t_loopInThisThread) {
    // 如果当前线程已经创建了EventLoop对象，则终止进程
    LOG_FATAL << "Another EventLoop has been created in this thread " << threadId_;
  } else {
    t_loopInThisThread = this;
  }
  wakeupChannel_->setReadCallback(boost::bind(&EventLoop::handleRead, this));
  wakeupChannel_->enableReading();
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
    doPendingFunctors();
  }

  LOG_INFO << "EventLoop " << this << " stop looping";
  looping_ = false;
}

void EventLoop::quit() {
  quit_ = true;
  wakeup();
}

void EventLoop::runInLoop(const Functor& cb) {
  if (isInLoopThread()) {
    cb();
  } else {
    queueInLoop(cb);
  }
}

void EventLoop::queueInLoop(const Functor& cb) {
  {
    MutexLockGuard lock(mutex_);
    pendingFunctors_.push_back(cb);
  }
  if (! isInLoopThread() || callingPendingFunctors_) wakeup();
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

void EventLoop::wakeup() {
  uint64_t one = 1;
  ssize_t n = write(wakeupfd_, &one, sizeof one);
  if (n != sizeof one) LOG_ERROR << "EventLoop::wakeup()";
}

void EventLoop::handleRead() {
  uint64_t one = 1;
  ssize_t n = read(wakeupfd_, &one, sizeof one);
  if (n != sizeof one) LOG_ERROR << "EventLoop::handleRead()";
}

void EventLoop::doPendingFunctors() {
  std::vector<Functor> functors;
  callingPendingFunctors_ = true;

  {
    MutexLockGuard lock(mutex_);
    functors.swap(pendingFunctors_);
  }

  for (size_t i = 0; i < functors.size(); i ++) functors[i]();

  callingPendingFunctors_ = false;
}

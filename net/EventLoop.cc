#include <net/EventLoop.h>
#include <base/Logging.h>
#include <poll.h>
#include <net/Poller.h>
#include <net/Channel.h>
#include <net/poller/PollPoller.h>

using namespace base;
using namespace base::net;

namespace {
  __thread EventLoop* t_loopInThisThread = NULL; // 线程局部存储
}

EventLoop::EventLoop()
  : looping_(false),
    threadId_(CurrentThread::tid()),
    poller_(new PollPoller(this))
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

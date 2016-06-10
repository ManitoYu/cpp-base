#include <net/EventLoopThreadPool.h>
#include <net/EventLoop.h>
#include <net/EventLoopThread.h>
#include <stdio.h>

using namespace base;
using namespace base::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const string& nameArg)
  : baseLoop_(baseLoop),
    name_(nameArg),
    started_(false),
    numThreads_(0),
    next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool() {
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb) {
  assert(! started_);
  baseLoop_->assertInLoopThread();

  started_ = true;

  for (int i = 0; i < numThreads_; i ++) {
    char buf[name_.size() + 32];
    snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
    EventLoopThread* t = new EventLoopThread(cb, buf);
    threads_.push_back(t);
    loops_.push_back(t->startLoop());
  }
  if (cb && numThreads_ == 0) {
    cb(baseLoop_);
  }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
  baseLoop_->assertInLoopThread();
  assert(started_);
  EventLoop* loop = baseLoop_;

  if (! loops_.empty()) {
    loop = loops_[next_];
    ++ next_;
    if (static_cast<size_t>(next_) >= loops_.size()) {
      next_ = 0;
    }
  }
  return loop;
}

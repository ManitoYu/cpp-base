#ifndef NET_EVENTLOOPTHREAD_H
#define NET_EVENTLOOPTHREAD_H

#include <base/Thread.h>
#include <boost/noncopyable.hpp>
#include <base/Mutex.h>
#include <base/Condition.h>
#include <base/Types.h>

namespace base {
namespace net {

class EventLoop;

class EventLoopThread : boost::noncopyable {
  public:
    typedef boost::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
      const string& name = string());
    ~EventLoopThread();
    EventLoop* startLoop();

  private:
    void threadFunc();

    EventLoop* loop_;
    bool exiting_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    ThreadInitCallback callback_;
};

}
}

#endif // NET_EVENTLOOPTHREAD_H

#ifndef NET_EVENTLOOPTHREADPOOL_H
#define NET_EVENTLOOPTHREADPOOL_H

#include <boost/noncopyable.hpp>
#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/function.hpp>
#include <base/Types.h>

namespace base {
namespace net {

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : boost::noncopyable {
  public:
    typedef boost::function<void (EventLoop*)> ThreadInitCallback;

    EventLoopThreadPool(EventLoop* baseLoop, const string& nameArg);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads) { numThreads_ = numThreads; }
    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    EventLoop* getNextLoop();

    bool started() const { return started_; }

    const string& name() const { return name_; }

  private:
    EventLoop* baseLoop_;
    string name_;
    bool started_;
    int numThreads_;
    int next_;
    boost::ptr_vector<EventLoopThread> threads_;
    std::vector<EventLoop*> loops_;
};

}
}

#endif

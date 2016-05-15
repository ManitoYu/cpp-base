#ifndef BASE_THREAD_H
#define BASE_THREAD_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <pthread.h>

namespace base {

class Thread : boost::noncopyable {
  public:
    typedef boost::function<void ()> ThreadFunc;

    explicit Thread(const ThreadFunc&);
    ~Thread();

    void start();
    int join();

    bool started() const { return started_; };

    void runInThread();

  private:
    pthread_t pthreadId_;
    bool started_;
    bool joined_;
    ThreadFunc func_;
};

}

#endif

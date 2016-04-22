#ifndef BASE_COUNTDOWNLATCH_H
#define BASE_COUNTDOWNLATCH_H

#include <base/Mutex.h>
#include <base/Condition.h>
#include <boost/noncopyable.hpp>

namespace base {

class CountDownLatch : boost::noncopyable {
  public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

  private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};

}

#endif

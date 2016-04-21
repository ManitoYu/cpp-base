#ifndef BASE_MUTEX_H
#define BASE_MUTEX_H

#include <base/CurrentThread.h>
#include <boost/noncopyable.hpp>
#include <pthread.h>

namespace base {

class MutexLock : boost::noncopyable {
  public:
    MutexLock() : holder_(0) {
      pthread_mutex_init(&mutex_, NULL);
    }

    ~MutexLock() {
      assert(holder_ == 0); // 如果锁还被持有，则失败
      pthread_mutex_destroy(&mutex_);
    }

    bool isLockedByThisThread() const {
      return holder_ == CurrentThread::tid();
    }

    void lock() {
      assignHolder();
      pthread_mutex_lock(&mutex_);
    }

    void unlock() {
      unassignHolder();
      pthread_mutex_unlock(&mutex_);
    }

    void assignHolder() {
      holder_ = CurrentThread::tid();
    }

    void unassignHolder() {
      holder_ = 0;
    }

  private:
    pthread_mutex_t mutex_;
    pid_t holder_;
};

class MutexLockGuard : boost::noncopyable {
  public:
    explicit MutexLockGuard(MutexLock& mutex) : mutex_(mutex) {
      mutex_.lock();
    }
    ~MutexLockGuard() {
      mutex_.unlock();
    }
  private:
    MutexLock& mutex_;
};

}

#endif

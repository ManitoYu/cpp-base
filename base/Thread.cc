#include <base/Thread.h>
#include <base/CurrentThread.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

namespace base {

namespace CurrentThread {
  __thread int t_cacheTid = 0;
  __thread char t_tidString[32];
  __thread int t_tidStringLength = 6;
  __thread const char* t_threadName = "unknown";
}

void* threadFunc(void* obj) {
  static_cast<Thread*>(obj)->runInThread();
}

namespace detail {
  pid_t gettid() {
    return static_cast<pid_t>(::syscall(SYS_gettid));
  }
}

}

using namespace base;

void CurrentThread::cacheTid() {
  if (t_cacheTid == 0) {
    t_cacheTid = detail::gettid();
    t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cacheTid);
  }
}

bool CurrentThread::isMainThread() {
  return tid() == ::getpid();
}

Thread::Thread(const ThreadFunc& func)
  : started_(false),
    joined_(false ),
    pthreadId_(0),
    func_(func)
{
}

Thread::~Thread() {
  if (started_ && ! joined_) pthread_detach(pthreadId_);
}

void Thread::start() {
  assert(! started_);
  started_ = true;
  if (pthread_create(&pthreadId_, NULL, &threadFunc, this)) {
    started_ = false;
    std::cout << "Failed in pthread_create" << std::endl;
  }
}

int Thread::join() {
  assert(started_);
  assert(! joined_);
  joined_ = true;
  return pthread_join(pthreadId_, NULL);
}

void Thread::runInThread() {
  func_();
}

#include <base/Thread.h>

namespace base {

void* threadFunc(void* obj) {
  static_cast<Thread*>(obj)->runInThread();
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

}

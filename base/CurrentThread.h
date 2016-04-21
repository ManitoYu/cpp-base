#ifndef BASE_CURRENTTHREAD_H
#define BASE_CURRENTTHREAD_H

namespace base {

namespace CurrentThread {

  extern __thread int t_cacheTid;
  extern __thread char t_tidString[32];
  extern __thread int t_tidStringLength;
  extern __thread const char* t_threadName;

  void cacheTid();

  inline int tid() {
    if (t_cacheTid == 0) cacheTid();
    return t_cacheTid;
  }

  inline int tidStringLength() {
    return t_tidStringLength;
  }

  inline const char* tidString() {
    return t_tidString;
  }

  inline const char* name() {
    return t_threadName;
  }

  bool isMainThread();
}

}

#endif

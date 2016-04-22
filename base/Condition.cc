#include <base/Condition.h>
#include <errno.h>

using namespace base;

bool Condition::waitForSeconds(double seconds) {
  struct timespec abstime;
  clock_gettime(CLOCK_MONOTONIC, &abstime);

  const int64_t kNanoSecondsPerSecond = 1e9; // 每秒纳秒数
  int64_t nanoSeconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond);

  abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoSeconds) / kNanoSecondsPerSecond);
  abstime.tv_nsec = (abstime.tv_nsec + nanoSeconds) % kNanoSecondsPerSecond;

  MutexLock::UnassignGuard ug(mutex_);
  return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);
}

#include <base/Timestamp.h>
#include <stdio.h>
#include <sys/time.h>

using namespace base;

string Timestamp::toString() const {
  char buf[32] = {0};
  int64_t seconds = microSeconds_ / kMicroSecondsPerSecond;
  int64_t microSeconds = microSeconds % kMicroSecondsPerSecond;
  snprintf(buf, sizeof(buf) - 1, "%ld.%06ld", seconds, microSeconds);
  return buf;
}

/**
 * 返回当前时间的Timestamp对象
 * @return Timestamp
 */
Timestamp Timestamp::now() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int64_t seconds = tv.tv_sec;
  return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

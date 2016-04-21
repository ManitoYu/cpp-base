#include <base/Timestamp.h>
#include <stdio.h>
#include <sys/time.h>
#include <boost/static_assert.hpp>

using namespace base;

BOOST_STATIC_ASSERT(sizeof(Timestamp) == sizeof(int64_t));

/**
 * 格式化为字符串
 * @return string
 */
string Timestamp::toString() const {
  char buf[32] = {0};
  int64_t seconds = microSeconds_ / kMicroSecondsPerSecond;
  int64_t microSeconds = microSeconds % kMicroSecondsPerSecond;
  snprintf(buf, sizeof(buf) - 1, "%ld.%06ld", seconds, microSeconds);
  return buf;
}

/**
 * 格式化时间
 * @return string
 */
string Timestamp::toFormattedString() const {
  char buf[32] = {0};
  time_t seconds = static_cast<time_t>(microSeconds_ / kMicroSecondsPerSecond);
  struct tm tmTime;
  localtime_r(&seconds, &tmTime);
  snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d",
    tmTime.tm_year + 1900, tmTime.tm_mon + 1, tmTime.tm_mday,
    tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec, microSeconds_);
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

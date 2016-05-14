#include <net/Timer.h>

using namespace base;
using namespace base::net;

AtomicInt64 Timer::s_numCreated_;

void Timer::restart(Timestamp now) {
  expiration_ = repeat_
    ? addTime(now, interval_) // Timestamp中定义addTime
    : Timestamp::invalid();
}

#ifndef BASE_TIMESTAMP_H
#define BASE_TIMESTAMP_H

#include <base/Types.h>
#include <boost/operators.hpp>

namespace base {

class Timestamp : public boost::less_than_comparable<Timestamp> {
public:
  Timestamp() : microSeconds_(0) {
  }
  explicit Timestamp(int64_t microSeconds) : microSeconds_(microSeconds) {
  }

  string toString() const;
  string toFormattedString() const;
  int64_t microSeconds() { return microSeconds_; }

  static Timestamp now();
  static Timestamp invalid() { return Timestamp(); }

  bool valid() const { return microSeconds_ > 0; }

  static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
  int64_t microSeconds_;
};

inline bool operator <(Timestamp t1, Timestamp t2) {
  return t1.microSeconds() < t2.microSeconds();
}

inline bool operator ==(Timestamp t1, Timestamp t2) {
  return t1.microSeconds() == t2.microSeconds();
}

inline double timeDifference(Timestamp high, Timestamp low) {
  int64_t diff = high.microSeconds() - low.microSeconds();
  return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

inline Timestamp addTime(Timestamp timestamp, double seconds) {
  int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
  return Timestamp(timestamp.microSeconds() + delta);
}

}

#endif

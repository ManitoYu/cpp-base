#ifndef BASE_TIMESTAMP_H
#define BASE_TIMESTAMP_H

#include <base/Types.h>

namespace base {


class Timestamp {
public:
  Timestamp() : microSeconds_(0) {

  }
  explicit Timestamp(int64_t microSeconds) : microSeconds_(microSeconds) {

  }

  string toString() const;

  static Timestamp now();

  static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
  int64_t microSeconds_;
};


}

#endif

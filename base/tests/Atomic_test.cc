#include <base/Atomic.h>
#include <assert.h>
#include <iostream>

using namespace base;
using namespace std;

int main() {

  AtomicInt64 a;
  assert(a.get() == 0);
  assert(a.incrementAndGet() == 1);

  return 0;
}

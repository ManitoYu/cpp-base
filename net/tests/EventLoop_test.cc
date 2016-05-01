#include <net/EventLoop.h>
#include <base/Thread.h>

using namespace base;
using namespace base::net;

void threadFunc() {
  EventLoop loop;
  loop.loop();
}

int main() {
  EventLoop loop;

  Thread t(threadFunc);
  t.start();

  loop.loop();

  t.join();

  return 0;
}

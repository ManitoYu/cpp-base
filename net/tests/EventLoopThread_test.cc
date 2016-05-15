#include <net/EventLoopThread.h>
#include <net/EventLoop.h>
#include <base/Logging.h>
#include <unistd.h>
#include <base/CurrentThread.h>

using namespace base;
using namespace base::net;

int main() {
  LOG_INFO << CurrentThread::tid();

  EventLoopThread t;

  EventLoop* loop = t.startLoop();

  loop->runInLoop([&loop]() {
    loop->runInLoop([]() { LOG_INFO << "cb"; });
    LOG_INFO << CurrentThread::tid(); // IO线程
    LOG_INFO << "print";
  });

  loop->runAfter(10, [&loop]() { loop->quit(); });

  sleep(60);

  return 0;
}

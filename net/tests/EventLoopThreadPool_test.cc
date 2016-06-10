#include <net/EventLoopThreadPool.h>
#include <net/EventLoop.h>
#include <base/Logging.h>
#include <boost/bind.hpp>

using namespace base;
using namespace base::net;

int main() {
  EventLoop loop;

  EventLoopThreadPool pool(&loop, "yucong");
  pool.setThreadNum(4);
  pool.start([](EventLoop*) {
    LOG_INFO << "thread is starting ...";
  });

  pool.getNextLoop()->runAfter(1, []() {
    LOG_INFO << CurrentThread::tid();
  });

  pool.getNextLoop()->runAfter(2, []() {
    LOG_INFO << CurrentThread::tid();
  });

  pool.getNextLoop()->runAfter(3, []() {
    LOG_INFO << CurrentThread::tid();
  });

  pool.getNextLoop()->runAfter(4, []() {
    LOG_INFO << CurrentThread::tid();
  });

  pool.getNextLoop()->runAfter(5, []() {
    LOG_INFO << CurrentThread::tid();
  });

  loop.loop();

  return 0;
}

#include <base/CountDownLatch.h>
#include <base/Thread.h>
#include <base/Mutex.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include <vector>
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace base;

MutexLock g_mutex;
int g = 0;

int main() {
  boost::ptr_vector<Thread> threads;

  CountDownLatch latch(1);

  for (int i = 0; i < 8; i++) {
    threads.push_back(new Thread([&latch]() {
      // MutexLockGuard lock(g_mutex);
      latch.wait();
      cout << g++ << endl;
    }));
    threads[i].start();
  }

  sleep(2);
  latch.countDown();

  for (int i = 0; i < 8; i++) {
    threads[i].join();
  }

  return 0;
}

#include <base/Thread.h>
#include <base/Mutex.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace base;

MutexLock g_mutex;
int g = 0;

int main() {
  boost::ptr_vector<Thread> threads;

  for (int i = 0; i < 8; i++) {
    threads.push_back(new Thread([]() {
      // MutexLockGuard lock(g_mutex);
      cout << g << endl;
    }));
    threads[i].start();
  }

  for (int i = 0; i < 8; i++) {
    threads[i].join();
  }

  return 0;
}

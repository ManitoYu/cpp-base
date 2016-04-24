#include <base/ThreadPool.h>
#include <base/CurrentThread.h>

using namespace std;
using namespace base;

void print() {
  cout << CurrentThread::tid() << endl;
}

int main() {

  cout << "start" << endl;

  ThreadPool pool("test");
  pool.start(8);

  for (int i = 0; i < 10; i ++) {
    pool.run(print);
  }

  cout << pool.queueSize() << endl;

  cin.get();

  // pool.stop();

  return 0;
}

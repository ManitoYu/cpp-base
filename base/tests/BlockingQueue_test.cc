#include <iostream>
#include <base/BlockingQueue.h>
#include <base/CountDownLatch.h>
#include <base/Thread.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include <functional>

using namespace std;
using namespace base;

class Test {
  public:
    Test(int num)
      : latch_(num),
        threads_(num)
    {
      for (int i = 0; i < num; i ++) {
        threads_.push_back(new Thread([&]() {
          latch_.countDown();
        }));
      }
    }
  private:
    boost::ptr_vector<Thread> threads_;
    CountDownLatch latch_;
};

int main() {


  return 0;
}

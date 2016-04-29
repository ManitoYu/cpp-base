#include <base/ThreadLocalSingleton.h>
#include <base/CurrentThread.h>
#include <base/Thread.h>
#include <iostream>

using namespace std;
using namespace base;

class Test {
  public:
    Test() {
      cout << "Test " << CurrentThread::tid() << endl;
    }

    ~Test() {
      cout << "~Test " << CurrentThread::tid() << endl;
    }

    void setName(const string& name) { name_ = name; }
    const string& name() const { return name_; }

  private:
    string name_;
};

void threadFunc() {
  ThreadLocalSingleton<Test>::instance().setName("thread");
  cout
    << "thread "
    << CurrentThread::tid()
    << " name = "
    << ThreadLocalSingleton<Test>::instance().name()
    << endl;
}

int main() {
  ThreadLocalSingleton<Test>::instance().setName("main");
  
  Thread t(threadFunc);
  t.start();
  t.join();

  cout
    << "main "
    << CurrentThread::tid()
    << " name = "
    << ThreadLocalSingleton<Test>::instance().name()
    << endl;

  return 0;
}

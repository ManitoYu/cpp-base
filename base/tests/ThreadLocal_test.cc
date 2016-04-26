#include <base/CurrentThread.h>
#include <base/ThreadLocal.h>
#include <base/Thread.h>

using namespace base;
using namespace std;

class Test {
  public:
    Test() {
      cout << "Test " << CurrentThread::tid() << endl;
    }
    ~Test() {
      cout << "~Test " << CurrentThread::tid() << endl;
    }

    const string& name() const { return name_; }
    void setName(const string& name) { name_ = name; }

  private:
    string name_;
};

ThreadLocal<Test> test;

void print() {
  cout << CurrentThread::tid() << " " << test.value().name() << endl;
}

void threadFunc() {
  print();
  test.value().setName("thread");
  print();
}

int main() {
  print();
  test.value().setName("main");
  print();

  Thread t(threadFunc);
  t.start();
  t.join();

  print();

  return 0;
}

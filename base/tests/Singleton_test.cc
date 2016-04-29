#include <iostream>
#include <base/Singleton.h>
#include <base/Thread.h>

using namespace std;
using namespace base;

class Test {
  public:
    Test() {
      cout << "Test" << endl;
    }

    ~Test() {
      cout << "~Test" << endl;
    }

    void print() {
      cout << "print " << name_ << endl;
    }

    void setName(const string& name) { name_ = name; }
    const string& name() { return name_; }

  private:
    string name_;
};

void func() {
  Singleton<Test>::instance().print();
}

int main() {
  Singleton<Test>::instance().print();
  Singleton<Test>::instance().setName("main");
  Singleton<Test>::instance().print();

  Thread t(func);
  t.start();
  t.join();

  return 0;
}

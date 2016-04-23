#include <iostream>
#include <base/Singleton.h>

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
      cout << "print" << endl;
    }
};

int main() {
  Singleton<Test>::instance().print();
  Singleton<Test>::instance().print();
  Singleton<Test>::instance().print();
  Singleton<Test>::instance().print();
  Singleton<Test>::instance().print();

  return 0;
}

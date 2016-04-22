#include <base/Exception.h>
#include <iostream>

using namespace std;
using namespace base;

void test() {
  throw Exception("test error");
}

int main() {

  try {
    test();
  } catch (const Exception& e) {
    cout << e.what() << endl;
    cout << e.stackTrace() << endl;
  }

  return 0;
}

#include <base/Timestamp.h>
#include <iostream>

using namespace base;
using namespace std;

int main() {

  Timestamp t = Timestamp::now();

  cout << t.toString() << endl;

  return 0;
}

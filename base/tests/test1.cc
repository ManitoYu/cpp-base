#include <base/Timestamp.h>
#include <iostream>

using namespace base;
using namespace std;

int main() {

  Timestamp t = Timestamp::now();

  cout << t.toString() << endl;
  cout << t.toFormattedString() << endl;
  cout << t.invalid().toString() << endl;

  cout << addTime(Timestamp::now(), 20).toString() << endl;

  cout << timeDifference(addTime(Timestamp::now(), 20), Timestamp::now()) << endl;

  return 0;
}

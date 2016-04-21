#include <iostream>
#include <base/Thread.h>
#include <algorithm>

using namespace std;
using namespace base;

int main() {

  Thread t([]() {

    vector<int> v;
    for (int i = 0; i < 10; i ++) {
      v.push_back(i);
    }

    for_each(v.begin(), v.end(), [](int i) {
      cout << i << endl;
    });

    cout << *find(v.begin(), v.end(), 3) << endl;

  });

  t.start();
  t.join();

  return 0;
}

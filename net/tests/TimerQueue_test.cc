#include <net/EventLoop.h>
#include <iostream>
#include <boost/bind.hpp>

using namespace std;
using namespace base;
using namespace base::net;

void print(const char* msg) {
  cout << msg << endl;
}

int main() {

  EventLoop loop;

  loop.runEvery(2, boost::bind(print, "every2"));
  loop.runAfter(1, boost::bind(print, "once1"));
  loop.runAfter(1.5, boost::bind(print, "once1.5"));
  loop.runAfter(2.5, boost::bind(print, "once2.5"));
  loop.runAfter(3.5, boost::bind(print, "once3.5"));
  TimerId t4 = loop.runAfter(4, boost::bind(print, "once4"));
  // loop.cancel(t4);

  loop.loop();

  return 0;
}

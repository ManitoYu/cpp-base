#include <net/Channel.h>
#include <net/EventLoop.h>
#include <boost/bind.hpp>
#include <base/Logging.h>
#include <unistd.h>

using namespace base;
using namespace base::net;

void print(Timestamp time) {
  char buf[64] = { 0 };
  ::read(0, buf, sizeof buf);
  LOG_INFO << time.toFormattedString() << " " << buf;
}

int main() {
  EventLoop loop;

  Channel channel(&loop, 0);
  channel.setReadCallback(boost::bind(print, _1));
  channel.enableReading();

  loop.loop();

  return 0;
}

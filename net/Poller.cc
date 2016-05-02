#include <net/Poller.h>

using namespace base;
using namespace base::net;

Poller::Poller(EventLoop* loop)
  : ownerLoop_(loop)
{
}

Poller::~Poller() {
}

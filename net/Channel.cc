#include <net/Channel.h>
#include <poll.h>
#include <net/EventLoop.h>

using namespace base;
using namespace base::net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI; // POLLPRI 紧急数据
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
  : loop_(loop),
    fd_(fd),
    index_(-1),
    events_(0),
    revents_(0),
    eventHandling_(false)
{
}

Channel::~Channel() {
}

void Channel::update() {
  loop_->updateChannel(this);
}

void Channel::remove() {
  assert(isNoneEvent());
  loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime) {
  handleEventWithGuard(receiveTime);
}

void Channel::handleEventWithGuard(Timestamp receiveTime) {
  eventHandling_ = true;

  // 挂断
  if ((revents_ & POLLHUP) && ! (revents_ & POLLIN)) {
    if (closeCallback_) closeCallback_();
  }

  // 文件描述符不合法
  if (revents_ & POLLNVAL) {
  }

  if (revents_ & (POLLERR | POLLNVAL)) {
    if (errorCallback_) errorCallback_();
  }

  // POLLRDHUP 关闭连接
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (readCallback_) readCallback_(receiveTime);
  }

  if (revents_ & POLLOUT) {
    if (writeCallback_) writeCallback_();
  }

  eventHandling_ = false;
}

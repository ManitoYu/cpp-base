#include <net/poller/EPollPoller.h>
#include <sys/epoll.h>
#include <base/Logging.h>
#include <net/Channel.h>
#include <poll.h>

using namespace base;
using namespace base::net;

namespace base {
namespace net {
  const int kNew = -1;
  const int kAdded = 1;
  const int kDeleted = 2;
}
}

BOOST_STATIC_ASSERT(EPOLLIN == POLLIN);
BOOST_STATIC_ASSERT(EPOLLPRI == POLLPRI);
BOOST_STATIC_ASSERT(EPOLLOUT == POLLOUT);
BOOST_STATIC_ASSERT(EPOLLRDHUP == POLLRDHUP);
BOOST_STATIC_ASSERT(EPOLLERR == POLLERR);
BOOST_STATIC_ASSERT(EPOLLHUP == POLLHUP);

EPollPoller::EPollPoller(EventLoop* loop)
  : Poller(loop),
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
    events_(kInitEventListSize)
{
  if (epollfd_ < 0) LOG_FATAL << "EPollPoller::EPollPoller";
}

EPollPoller::~EPollPoller() {
  ::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activeChannels) {
  int numEvents = ::epoll_wait(
    epollfd_,
    &*events_.begin(),
    static_cast<int>(events_.size()),
    timeoutMs);

  Timestamp now(Timestamp::now());

  if (numEvents > 0) {
    fillActiveChannels(numEvents, activeChannels);
    if (static_cast<size_t>(numEvents) == events_.size()) {
      events_.resize(events_.size() * 2);
    } else if (numEvents == 0) {
      LOG_TRACE << "nothing happended";
    } else {
      LOG_ERROR << "EPollPoller::poll()";
    }
  }

  return now;
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
  for (int i = 0; i < numEvents; i ++) {
    Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
    channel->setRevents(events_[i].events);
    activeChannels->push_back(channel);
  }
}

void EPollPoller::updateChannel(Channel* channel) {
  const int index = channel->index();
  int fd = channel->fd();

  if (index == kNew || index == kDeleted) {
    if (index == kNew) {
      assert(channels_.find(fd) == channels_.end());
      channels_[fd] = channel;
    } else {
      assert(channels_.find(fd) != channels_.end());
      assert(channels_[fd] = channel);
    }
    channel->setIndex(kAdded);
    update(EPOLL_CTL_ADD, channel);
  } else {
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(index == kAdded);
    if (channel->isNoneEvent()) {
      update(EPOLL_CTL_DEL, channel);
      channel->setIndex(kDeleted);
    } else {
      update(EPOLL_CTL_MOD, channel);
    }
  }
}

void EPollPoller::removeChannel(Channel* channel) {
  int fd = channel->fd();
  assert(channels_.find(fd) != channels_.end());
  assert(channels_[fd] == channel);
  assert(channel->isNoneEvent());
  int index = channel->index();
  assert(index == kAdded || index == kDeleted);
  size_t n = channels_.erase(fd);
  assert(1 == n);
  if (index == kAdded) update(EPOLL_CTL_DEL, channel);
  channel->setIndex(kNew);
}

void EPollPoller::update(int operation, Channel* channel) {
  struct epoll_event event;
  ::bzero(&event, sizeof event);
  event.events = channel->events();
  event.data.ptr = channel;
  int fd = channel->fd();
  if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
    LOG_ERROR << "EPollPoller::update";
  }
}

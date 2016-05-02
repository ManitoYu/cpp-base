#include <net/poller/PollPoller.h>
#include <net/Channel.h>
#include <poll.h>

using namespace base;
using namespace base::net;

PollPoller::PollPoller(EventLoop* loop)
  : Poller(loop)
{
}

PollPoller::~PollPoller()
{
}

Timestamp PollPoller::poll(int timeoutMs, ChannelList* activeChannels) {
  int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
  Timestamp now(Timestamp::now());
  if (numEvents > 0) {
    fillActiveChannels(numEvents, activeChannels);
  }
  return now;
}

void PollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
  for (PollFdList::const_iterator it = pollfds_.begin();
    it != pollfds_.end() && numEvents > 0; it ++)
  {
    if (it->revents > 0) {
      numEvents --;
      ChannelMap::const_iterator ch = channels_.find(it->fd);
      assert(ch != channels_.end()); // fd已注册
      Channel* channel = ch->second;
      assert(channel->fd() == it->fd); // 注册的fd和channel所持有的fd相等
      channel->setRevents(it->revents);
      activeChannels->push_back(channel);
    }
  }
}

void PollPoller::updateChannel(Channel* channel) {
  Poller::assertInLoopThread();

  if (channel->index() < 0) {
    // 新建
    assert(channels_.find(channel->fd()) == channels_.end());
    struct pollfd pfd;
    pfd.fd = channel->fd();
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    pollfds_.push_back(pfd);
    int idx = static_cast<int>(pollfds_.size() - 1);
    channel->setIndex(idx);
    channels_[pfd.fd] = channel;
  } else {
    // 更新
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    int idx = channel->index();
    assert(idx >= 0 && idx < static_cast<int>(pollfds_.size()));
    struct pollfd& pfd = pollfds_[idx];
    assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd() - 1);
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    if (channel->isNoneEvent()) {
      // 暂时忽略该文件描述符的事件
      pfd.fd = -channel->fd() - 1; // 为了removeChannel优化
    }
  }
}

void PollPoller::removeChannel(Channel* channel) {
  Poller::assertInLoopThread();
  assert(channels_.find(channel->fd()) != channels_.end()); // channel存在
  assert(channels_[channel->fd()] == channel);
  assert(channel->isNoneEvent()); // 没有关注任何事件
  int idx = channel->index();
  assert(idx >= 0 && idx < static_cast<int>(pollfds_.size()));
  const struct pollfd& pfd = pollfds_[idx];
  assert(pfd.fd == -channel->fd() - 1 && pfd.events == channel->events());
  size_t n = channels_.erase(channel->fd());
  assert(n == 1);
  if (static_cast<size_t>(idx) == pollfds_.size() - 1) {
    pollfds_.pop_back();
  } else {
    int channelAtEnd = pollfds_.back().fd;
    iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1); // 与最后一个元素交换，再删除
    if (channelAtEnd < 0) {
      channelAtEnd = -channelAtEnd - 1;
    }
    channels_[channelAtEnd]->setIndex(idx);
    pollfds_.pop_back();
  }
}

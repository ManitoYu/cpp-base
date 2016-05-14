#ifndef NET_CHANNEL_H
#define NET_CHANNEL_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <base/Timestamp.h>

namespace base {
namespace net {

class EventLoop;

class Channel : boost::noncopyable {
  public:
    typedef boost::function<void ()> EventCallback;
    typedef boost::function<void (Timestamp)> ReadEventCallback;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent(Timestamp receiveTime);

    void setReadCallback(const ReadEventCallback& cb) { readCallback_ = cb; }
    void setWriteCallback(const EventCallback& cb) { writeCallback_ = cb; }
    void setCloseCallback(const EventCallback& cb) { closeCallback_ = cb; }
    void setErrorCallback(const EventCallback& cb) { errorCallback_ = cb; }

    int fd() const { return fd_; }
    int events() const { return events_; }
    int index() const { return index_; }

    void setRevents(int revents) { revents_ = revents; }
    void setIndex(int idx) { index_ = idx; }

    bool isNoneEvent() const { return events_ == kNoneEvent; }

    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ != kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    EventLoop* ownerLoop() const { return loop_; }
    void remove();

  private:
    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int fd_;
    int events_; // 注册的事件
    int revents_; // 返回的事件
    int index_;

    bool eventHandling_;
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

}

}

#endif

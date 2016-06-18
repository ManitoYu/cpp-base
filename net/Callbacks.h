#ifndef NET_CALLBACKS_H
#define NET_CALLBACKS_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <base/Timestamp.h>

namespace base {
namespace net {

class Buffer;
class TcpConnection;

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef boost::function<void ()> TimerCallback;
typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef boost::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef boost::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef boost::function<void (const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;
typedef boost::function<void (const TcpConnectionPtr&, Buffer*, Timestamp)> MessageCallback;

void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime);

}
}

#endif // NET_CALLBACKS_H

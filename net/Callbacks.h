#ifndef NET_CALLBACKS_H
#define NET_CALLBACKS_H

#include <boost/function.hpp>

namespace base {
namespace net {

typedef boost::function<void ()> TimerCallback;

}
}

#endif // NET_CALLBACKS_H

cmake_minimum_required(VERSION 2.8)

set(
  net
  EventLoop.cc
  Channel.cc
  Poller.cc
  poller/PollPoller.cc
  poller/EPollPoller.cc
  Timer.cc
  TimerQueue.cc
  EventLoopThread.cc
  InetAddress.cc
  SocketsOps.cc
  Socket.cc
  Acceptor.cc
  EventLoopThreadPool.cc
)

add_library(net ${net})
target_link_libraries(net base) # net依赖于base

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/net)
install(TARGETS net DESTINATION lib)

add_subdirectory(tests)

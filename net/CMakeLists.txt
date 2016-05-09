cmake_minimum_required(VERSION 2.8)

set(
  net
  EventLoop.cc
  Channel.cc
  Poller.cc
  poller/PollPoller.cc
  poller/EPollPoller.cc
)

add_library(net ${net})
target_link_libraries(net base) # net依赖于base
install(TARGETS net DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/net)

add_subdirectory(tests)

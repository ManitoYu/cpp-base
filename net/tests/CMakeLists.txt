add_executable(EventLoop_test EventLoop_test.cc)
target_link_libraries(EventLoop_test net pthread)

add_executable(Channel_test Channel_test.cc)
target_link_libraries(Channel_test net pthread)

add_executable(TimerQueue_test TimerQueue_test.cc)
target_link_libraries(TimerQueue_test net pthread)

add_executable(EventLoopThread_test EventLoopThread_test.cc)
target_link_libraries(EventLoopThread_test net pthread)
set_target_properties(EventLoopThread_test PROPERTIES COMPILE_FLAGS "-std=c++11")

add_executable(InetAddress_test InetAddress_test.cc)
target_link_libraries(InetAddress_test net boost_unit_test_framework)

add_executable(Acceptor_test Acceptor_test.cc)
target_link_libraries(Acceptor_test net pthread)
set_target_properties(Acceptor_test PROPERTIES COMPILE_FLAGS "-std=c++11")

add_executable(EventLoopThreadPool_test EventLoopThreadPool_test.cc)
target_link_libraries(EventLoopThreadPool_test net pthread)
set_target_properties(EventLoopThreadPool_test PROPERTIES COMPILE_FLAGS "-std=c++11")

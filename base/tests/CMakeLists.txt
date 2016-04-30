add_executable(test1 test1.cc)
target_link_libraries(test1 base)

add_executable(test2 test2.cc)
target_link_libraries(test2 base pthread)
set_target_properties(test2 PROPERTIES COMPILE_FLAGS "-std=c++0x")

add_executable(Mutex_test Mutex_test.cc)
target_link_libraries(Mutex_test base pthread)
set_target_properties(Mutex_test PROPERTIES COMPILE_FLAGS "-std=c++0x")

add_executable(Exception_test Exception_test.cc)
target_link_libraries(Exception_test base)

add_executable(Singleton_test Singleton_test.cc)
target_link_libraries(Singleton_test base pthread)

add_executable(ThreadPool_test ThreadPool_test.cc)
target_link_libraries(ThreadPool_test base pthread)
set_target_properties(ThreadPool_test PROPERTIES COMPILE_FLAGS "-std=c++0x")

add_executable(ThreadLocal_test ThreadLocal_test.cc)
target_link_libraries(ThreadLocal_test base pthread)

add_executable(ThreadLocalSingleton_test ThreadLocalSingleton_test.cc)
target_link_libraries(ThreadLocalSingleton_test base pthread)

add_executable(Logging_test Logging_test.cc)
target_link_libraries(Logging_test base)

add_executable(LogStream_test LogStream_test.cc)
target_link_libraries(LogStream_test base boost_unit_test_framework)

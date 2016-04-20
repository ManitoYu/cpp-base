add_executable(test1 test1.cc)
target_link_libraries(test1 base)

add_executable(test2 test2.cc)
target_link_libraries(test2 base pthread)
set_target_properties(test2 PROPERTIES COMPILE_FLAGS "-std=c++0x")

cmake_minimum_required(VERSION 2.8)

set(
  base
  Timestamp.cc
  Thread.cc
  Condition.cc
  CountDownLatch.cc
)

add_library(base ${base})
install(TARGETS base DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/base)

add_subdirectory(tests)

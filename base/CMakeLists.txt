set(
  base
  Timestamp.cc
  Thread.cc
  Condition.cc
  CountDownLatch.cc
  Exception.cc
  ThreadPool.cc
  LogStream.cc
  LogFile.cc
  FileUtil.cc
  Logging.cc
)

add_library(base ${base})
install(TARGETS base DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/base)

add_subdirectory(tests)

set(
  SRCS
  HttpServer.cc
  HttpResponse.cc
  HttpContext.cc
)

add_library(http ${SRCS})
target_link_libraries(http net)

add_executable(Http_test tests/Http_test.cc)
target_link_libraries(Http_test http pthread)
set_target_properties(Http_test PROPERTIES COMPILE_FLAGS "-std=c++11")

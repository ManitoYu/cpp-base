#include <base/Logging.h>

using namespace base;

int main() {
  Logger::setLogLevel(Logger::DEBUG);

  LOG_DEBUG << "yucong";
  LOG_ERROR << "yucong";
  LOG_WARN << "yucong";
  LOG_INFO << "yucong";
  LOG_FATAL << "yucong";

  return 0;
}

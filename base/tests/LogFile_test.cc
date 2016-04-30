#include <base/LogFile.h>

using namespace base;

int main() {

  LogFile file("file", 64);

  file.append("yucong", 6);

  return 0;
}

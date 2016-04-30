#ifndef BASE_FILEUTIL_H
#define BASE_FILEUTIL_H

#include <boost/noncopyable.hpp>
#include <base/Types.h>

namespace base {

namespace FileUtil {

class AppendFile : boost::noncopyable {
  public:
    explicit AppendFile(const string& filename);
    ~AppendFile();
    void append(const char* logline, const size_t len);
    void flush();
    size_t writtenBytes() const { return writtenBytes_; }
  private:
    size_t write(const char* logline, size_t len);

    FILE* fp_;
    char buffer_[64 * 1024];
    size_t writtenBytes_;
};

}

}

#endif

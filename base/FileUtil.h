#ifndef BASE_FILEUTIL_H
#define BASE_FILEUTIL_H

#include <boost/noncopyable.hpp>

namespace base {

namespace FileUtil {

class AppendFile : boost::noncopyable {
  public:
    explicit AppendFile(const string& filename);
    ~AppendFile;
    void append(const char* logline, const size_t len);
    void flush();
    size_t writtemBytes() const { return writtemBytes_; }
  private:
    FILE* fp_;
    char buffer_[64 * 1024];
    size_t writtemBytes_;
}

}

}

#endif

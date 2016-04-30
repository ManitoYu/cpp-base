#ifndef BASE_LOGFILE_H
#define BASE_LOGFILE_H

#include <base/Mutex.h>
#include <base/Types.h>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace base {

namespace FileUtil {
  class AppendFile;
}

class LogFile : boost::noncopyable {
  public:
    LogFile(
      const string& basename,
      size_t rollSize,
      bool threadSafe = true,
      int flushInterval = 3);
    ~LogFile();

    void append(const char* logline, int len);
    void flush();
    bool rollFile();

  private:
    void appendUnlocked(const char* logline, int len);
    static string getLogFileName(const string& basename, time_t* now);

    const string basename_;
    const size_t rollSize_;
    const int flushInterval_; // 间隔一段时间才会集中写入

    int count_;

    boost::scoped_ptr<MutexLock> mutex_;
    time_t startOfPeriod_; // 零点时间
    time_t lastRoll_;
    time_t lastFlush_;
    boost::scoped_ptr<FileUtil::AppendFile> file_;

    const static int kRollPerSeconds = 60 * 60 * 24; // 一天
};

}

#endif

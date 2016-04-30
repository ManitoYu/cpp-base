#include <base/LogFile.h>
#include <base/FileUtil.h>
#include <assert.h>
#include <time.h>

using namespace base;

LogFile::LogFile(
  const string& basename,
  size_t rollSize,
  bool threadSafe,
  int flushInterval)
  : basename_(basename),
    rollSize_(rollSize),
    flushInterval_(flushInterval),
    count_(0),
    mutex_(threadSafe ? new MutexLock : NULL),
    startOfPeriod_(0),
    lastRoll_(0),
    lastFlush_(0)
{
  // 能不能找到斜杠
  assert(basename.find('/') == string::npos);
  rollFile();
}

LogFile::~LogFile() {
}

void LogFile::append(const char* logline, int len) {
  if (mutex_) {
    MutexLockGuard lock(*mutex_);
    appendUnlocked(logline, len);
  } else {
    appendUnlocked(logline, len);
  }
}

void LogFile::flush() {
  if (mutex_) {
    MutexLockGuard lock(*mutex_);
    file_->flush();
  } else {
    file_->flush();
  }
}

void LogFile::appendUnlocked(const char* logline, int len) {
  file_->append(logline, len);

  if (file_->writtenBytes() > rollSize_) {
    rollFile();
  } else {
    count_ ++;
    // FIXME
    if (count_ > 1024) {
      count_ = 0;
      time_t now = ::time(NULL);
      time_t thisPeriod = now / kRollPerSeconds * kRollPerSeconds;
      if (thisPeriod != startOfPeriod_) { // 新的一天
        rollFile();
      } else if (now - lastFlush_ > flushInterval_) { // 达到刷新间隔
        lastFlush_ = now;
        file_->flush();
      }
    }
  }
}

bool LogFile::rollFile() {
  time_t now = 0;
  string filename = getLogFileName(basename_, &now);
  time_t start = now / kRollPerSeconds * kRollPerSeconds; // 向下取整操作，调整至当天零点

  // 滚动日志
  if (now > lastRoll_) {
    lastRoll_ = now;
    lastFlush_ = now;
    startOfPeriod_ = start;
    file_.reset(new FileUtil::AppendFile(filename)); // 产生一个新文件
    return true;
  }

  return false;
}

string LogFile::getLogFileName(const string& basename, time_t* now) {
  string filename;
  filename.reserve(basename.size() + 64);
  filename = basename;

  char timebuf[32];
  struct tm tm;
  *now = time(NULL);
  localtime_r(now, &tm); // 线程安全，保存在tm中，并不使用返回值
  strftime(timebuf, sizeof timebuf, "%Y%m%d%H%M%S", &tm);
  filename += ".";
  filename += timebuf;
  filename += ".log";

  return filename;
}

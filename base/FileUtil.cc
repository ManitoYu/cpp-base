#include <base/FileUtil.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

using namespace base;

FileUtil::AppendFile::AppendFile(const string& filename)
  : fp_(::fopen(filename.c_str(), "ae")),
    writtenBytes_(0)
{
  assert(fp_);
  ::setbuffer(fp_, buffer_, sizeof buffer_); // 设置文件缓冲区
}

FileUtil::AppendFile::~AppendFile() {
  ::fclose(fp_);
}

void FileUtil::AppendFile::append(const char* logline, const size_t len) {
  size_t n = write(logline, len);
  size_t remain = len - n;
  while (remain > 0) {
    size_t x = write(logline + n, remain);
    if (x == 0) {
      int err = ferror(fp_);
      if (err) fprintf(stderr, "AppendFile::append() failed %s\n", strerror(err));
      break;
    }
    n += x;
    remain = len - n;
  }
  writtenBytes_ += len;
}

void FileUtil::AppendFile::flush() {
  ::fflush(fp_);
}

size_t FileUtil::AppendFile::write(const char* logline, size_t len) {
  return ::fwrite_unlocked(logline, 1, len, fp_); // 效率更高，不加锁
}

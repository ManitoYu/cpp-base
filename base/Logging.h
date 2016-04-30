#ifndef BASE_LOGGING_H
#define BASE_LOGGING_H

#include <base/Timestamp.h>

namespace base {

class Logger {
  public:
    // 日志级别
    enum LogLevel {
      TRACE,
      DEBUG,
      INFO,
      WARN,
      ERROR,
      FATAL,
      NUM_LOG_LEVELS
    };

    class SourceFile {
      public:
        // 获取路径basename
        explicit SourceFile(const char* filename)
          : data_(filename)
        {
          const char* slash = strrchr(filename, '/');
          if (slash) data_ = slash + 1;
          size_ = static_cast<int>(strlen(data_));
        }

      const char* data_;
      int size_;
    };

    LogStream& stream() { return impl_.stream_; }

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    ~Logger();

    static LogLevel logLevel();

  private:
    class Impl {
      public:
        typedef Logger::LogLevel LogLevel;
        Impl(LogLevel level, int oldErrno, const SourceFile& file, int line);

        Timestamp time_;
        LogLevel level_;
        LogStream stream_;
        SourceFile basename_;
    };

    Impl impl_;
};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel() {
  return g_logLevel;
}

#define LOG_TRACE if (Logger::logLever() <= Logger::TRACE) \
  Logger::(__FILE__, __LINE__, Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (Logger::logLevel() <= Logger::DEBUG) \
  Logger::(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO if (Logger::logLevel() <= Logger::INFO) \
  Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__ ,Logger::FATAL).stream()

}

#endif

#ifndef BASE_LOGSTREAM_H
#define BASE_LOGSTREAM_H

#include <base/Types.h>
#include <boost/noncopyable.hpp>
#include <string.h>

namespace base {

namespace detail {

const int kSmallBuffer = 4096;
const int kLargeBuffer = 4096 * 1024;

template<int SIZE>
class FixedBuffer : boost::noncopyable {
  public:
    FixedBuffer()
      : cur_(data_)
    {
    }

    void append(const char* buf, size_t len) {
      // implicit_cast
      if (static_cast<size_t>(avail()) > len) {
        memcpy(cur_, buf, len);
        cur_ += len;
      }
    }

    const char* data() const { return data_; }
    char* current() { return cur_; }

    int length() const { return static_cast<int>(cur_ - data_); }
    int avail() const { return static_cast<int>(end() - cur_); }

    void add(size_t len) { cur_ += len; }
    void reset() { cur_ = data_; }
    void bzero() { ::bzero(data_, sizeof data_); }

    string toString() const { return string(data_, length()); }

  private:
    const char* end() const { return data_ + sizeof data_; }
    char data_[SIZE];
    char* cur_;
};

}

class LogStream : boost::noncopyable {
    typedef LogStream self;
  public:
    typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;

    self& operator<<(bool v) {
      buffer_.append(v ? "1" : "0", 1);
      return *this;
    }

    self& operator<<(short);
    self& operator<<(unsigned short);
    self& operator<<(int);
    self& operator<<(unsigned int);
    self& operator<<(long);
    self& operator<<(unsigned long);
    self& operator<<(long long);
    self& operator<<(unsigned long long);

    self& operator<<(const void*);

    self& operator<<(float v) {
      *this << static_cast<double>(v);
      return *this;
    }
    self& operator<<(double v);

    self& operator<<(char v) {
      buffer_.append(&v, 1);
      return *this;
    }

    self& operator<<(const char* str) {
      if (str) {
        buffer_.append(str, strlen(str));
      } else {
        buffer_.append("(null)", 6);
      }
      return *this;
    }

    self& operator<<(const unsigned char* str) {
      return operator<<(reinterpret_cast<const char*>(str));
    }

    self& operator<<(const string& str) {
      buffer_.append(str.c_str(), str.size());
      return *this;
    }

    void append(const char* data, int len) { buffer_.append(data, len); }
    const Buffer& buffer() const { return buffer_; }
    void resetBuffer() { buffer_.reset(); }

  private:
    template<typename T>
    void formatInteger(T);

    Buffer buffer_;

    static const int kMaxNumericSize = 32;
};

}

#endif

#ifndef BASE_EXCEPTION_H
#define BASE_EXCEPTION_H

#include <base/Types.h>
#include <exception>

namespace base {

class Exception : public std::exception {
  public:
    Exception(const char* what);
    Exception(const string& what);
    virtual ~Exception() throw();
    virtual const char* what() const throw();
    const char* stackTrace() const throw();

  private:
    void fillStackTrace();

    string message_;
    string stack_;
};

}

#endif

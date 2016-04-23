#ifndef BASE_SINGLETON_H
#define BASE_SINGLETON_H

#include <boost/noncopyable.hpp>
#include <assert.h>
#include <pthread.h>

namespace base {

template<typename T>
class Singleton : boost::noncopyable {
  public:
    static T& instance() {
      pthread_once(&ponce_, &Singleton::init); // 如果ponce_被填充，则不会再调用
      assert(value_ != NULL);
      return *value_;
    }

  private:
    Singleton();
    ~Singleton();

    static void init() {
      value_ = new T();
    }

    static void destroy() {
      delete value_;
      value_ = NULL;
    }

  private:
    static pthread_once_t ponce_;
    static T* value_;
};

template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;

}

#endif

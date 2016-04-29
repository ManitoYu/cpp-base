#ifndef BASE_THREADLOCALSINGLETON_H
#define BASE_THREADLOCALSINGLETON_H

#include <boost/noncopyable.hpp>
#include <assert.h>
#include <pthread.h>

namespace base {

template<typename T>
class ThreadLocalSingleton : boost::noncopyable {
  public:
    static T& instance() {
      if (! t_value_) {
        t_value_ = new T();
        deleter_.set(t_value_);
      }
      return *t_value_;
    }

    static T* pointer() {
      return t_value_;
    }

  private:
    ThreadLocalSingleton();
    ~ThreadLocalSingleton();

    static void destructor(void* obj) {
      assert(obj == t_value_);
      delete t_value_;
      t_value_ = 0;
    }

    // 嵌套类，自动销毁指针所指对象
    class Deleter {
      public:
        Deleter() {
          pthread_key_create(&pkey_, &ThreadLocalSingleton::destructor);
        }
        ~Deleter() {
          pthread_key_delete(pkey_);
        }

        void set(T* newObj) {
          assert(pthread_getspecific(pkey_) == NULL);
          pthread_setspecific(pkey_, newObj);
        }

        pthread_key_t pkey_;
    };

    static __thread T* t_value_; // 每个线程都会有独有的未初始化的该成员
    static Deleter deleter_;
};

template<typename T>
__thread T* ThreadLocalSingleton<T>::t_value_ = 0;

template<typename T>
typename ThreadLocalSingleton<T>::Deleter ThreadLocalSingleton<T>::deleter_;

}

#endif

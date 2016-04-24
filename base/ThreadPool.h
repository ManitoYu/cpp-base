#ifndef BASE_THREADPOOL_H
#define BASE_THREADPOOL_H

#include <boost/noncopyable.hpp>
#include <base/Mutex.h>
#include <base/Condition.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/function.hpp>
#include <base/Types.h>
#include <base/Thread.h>
#include <deque>

namespace base {

class ThreadPool : boost::noncopyable {
  public:
    typedef boost::function<void ()> Task;

    explicit ThreadPool(const string& name = string("ThreadPool"));
    ~ThreadPool();

    void start(int numThreads); // 启动线程个数固定
    void stop(); // 关闭
    void run(const Task& t); // 运行任务

    const string& name() const { return name_; }
    size_t queueSize() const;

  private:
    bool isFull() const;
    void runInThread(); // 线程执行函数
    Task take(); // 获取任务

    mutable MutexLock mutex_;
    Condition notEmpty_; // 用来唤醒线程
    Condition notFull_;
    string name_;
    boost::ptr_vector<base::Thread> threads_; // 线程队列
    std::deque<Task> queue_; // 任务队列
    bool running_; // 线程池是否处于运行状态
};

}

#endif

#include <base/ThreadPool.h>
#include <base/Thread.h>
#include <boost/bind.hpp>
#include <base/Exception.h>
#include <iostream>

using namespace base;

ThreadPool::ThreadPool(const string& name)
  : mutex_(),
    notEmpty_(mutex_),
    notFull_(mutex_),
    name_(name),
    running_(false)
{
}

ThreadPool::~ThreadPool() {
  if (running_) stop();
}

void ThreadPool::run(const Task& task) {
  // 如果线程池为空，则直接执行任务
  if (threads_.empty()) {
    task();
  } else {
    MutexLockGuard lock(mutex_);
    queue_.push_back(task);
    notEmpty_.notify();
  }
}

void ThreadPool::start(int numThreads) {
  assert(threads_.empty());
  running_ = true;
  threads_.reserve(numThreads); // 扩展空间
  for (int i = 0; i < numThreads; i ++) {
    threads_.push_back(new Thread(boost::bind(&ThreadPool::runInThread, this)));
    threads_[i].start();
  }
}

void ThreadPool::stop() {
  {
    MutexLockGuard lock(mutex_);
    running_ = false;
    notEmpty_.notifyAll(); // 等待的线程解除阻塞
  }
  for_each(threads_.begin(), threads_.end(), boost::bind(&Thread::join, _1)); // 等待线程退出
}

ThreadPool::Task ThreadPool::take() {
  MutexLockGuard lock(mutex_);
  while (queue_.empty() && running_) {
    notEmpty_.wait(); // 等待任务
  }
  // 有任务到来
  Task task;
  if (! queue_.empty()) {
    task = queue_.front(); // 取出任务
    queue_.pop_front();
  }
  return task;
}

size_t ThreadPool::queueSize() const {
  MutexLockGuard lock(mutex_);
  return queue_.size();
}

void ThreadPool::runInThread() {
  try {
    while (running_) {
      Task task(take()); // 获取任务
      if (task) task();
    }
  } catch (const Exception& e) {

  }
}

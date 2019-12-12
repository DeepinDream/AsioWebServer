#pragma once
#include <functional>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "LogStream.h"


class AsyncLogger
{
public:
  AsyncLogger(const std::string basename, int flushInterval = 2);
  ~AsyncLogger()
  {
    if (running_)
      stop();
  }
  void append(const char *logline, int len);

  void start()
  {
    running_ = true;
    thread_.detach();
  }

  void stop()
  {
    running_ = false;
    cond_.notify_all();
    thread_.join();
  }

private:
  AsyncLogger(const AsyncLogger &) = delete;
  const AsyncLogger &operator=(const AsyncLogger &) = delete;
  void threadFunc();

private:
  typedef FixedBuffer<kLargeBuffer> Buffer;
  typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
  typedef std::shared_ptr<Buffer> BufferPtr;
  const int flushInterval_;
  bool running_;
  std::string basename_;
  std::thread thread_;
  std::mutex mutex_;
  std::condition_variable cond_;
  BufferPtr currentBuffer_;
  BufferPtr nextBuffer_;
  BufferVector buffers_;
  std::condition_variable thread_launch_;
};
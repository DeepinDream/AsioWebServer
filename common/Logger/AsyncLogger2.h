#pragma once
#include <functional>
#include <string>

#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>
#include "BufferSwaper.h"
#include "ThreadGuard.h"

class AsyncLogger2
{
public:
  AsyncLogger2(const std::string basename, int flushInterval = 2);
  ~AsyncLogger2();
  void append(const char *logline, int len);

  void start();

  void stop();

private:
  AsyncLogger2(const AsyncLogger2 &) = delete;
  const AsyncLogger2 &operator=(const AsyncLogger2 &) = delete;
  void threadFunc();

private:

  const int flushInterval_;
  std::atomic<bool> running_;
  std::string basename_;
  std::unique_ptr<ThreadGuard> thread_;
  std::mutex mutex_;
  std::condition_variable cond_;
  std::unique_ptr<BufferSwaper> swaper_;
};

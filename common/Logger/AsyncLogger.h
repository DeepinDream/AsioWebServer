#pragma once
#include "LogFileSlicer.h"
#include "LogStream.h"
#include "ThreadGuard.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class AsyncLogger {
  public:
    AsyncLogger(const std::string basename, int flushInterval = 2);
    ~AsyncLogger();
    void append(const char* logline, int len);

    void start();

    void stop();

  private:
    AsyncLogger(const AsyncLogger&) = delete;
    const AsyncLogger& operator=(const AsyncLogger&) = delete;
    void threadFunc();

  private:
    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
    typedef std::shared_ptr<Buffer> BufferPtr;

    const int flushInterval_;
    std::atomic<bool> running_;
    std::string basename_;
    //  ThreadGuard thread_;
    std::unique_ptr<ThreadGuard> thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
    LogFileSlicer fileSlicer_;
};

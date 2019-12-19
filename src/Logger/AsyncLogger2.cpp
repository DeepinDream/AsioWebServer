#include "AsyncLogger2.h"
#include <assert.h>
// #include <stdio.h>
// #include <unistd.h>
#include <functional>
#include "LogFile.h"

AsyncLogger2::AsyncLogger2(std::string logFileName_, int flushInterval)
    : flushInterval_(flushInterval),
      running_(false),
      basename_(logFileName_)
{
  assert(logFileName_.size() > 1);

  thread_.reset(new ThreadGuard(std::bind(&AsyncLogger2::threadFunc, this)));
  swaper_.reset(new BufferSwaper);
}

AsyncLogger2::~AsyncLogger2()
{
    if (running_)
        stop();
}

void AsyncLogger2::append(const char *logline, int len)
{
    std::unique_lock<std::mutex> lock(mutex_);
  if (swaper_->avail() > len)
    swaper_->append(logline, len);
  else
  {
    swaper_->pushBuffer();
    swaper_->append(logline, len);
    cond_.notify_all();
  }
}

void AsyncLogger2::start()
{
    running_ = true;
    thread_->detach();
}

void AsyncLogger2::stop()
{
    running_ = false;
    cond_.notify_all();
    thread_->waitForStop();
}

void AsyncLogger2::threadFunc()
{
    assert(running_ == true);
    LogFile output(basename_);
    while (running_)
    {
        // assert(newBuffer1 && newBuffer1->length() == 0);
        // assert(newBuffer2 && newBuffer2->length() == 0);
        // assert(buffersToWrite.empty());

        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (swaper_->isVectorEmpty()) // unusual usage!
            {
                cond_.wait_for(lock, std::chrono::seconds(flushInterval_));
            }
            swaper_->swap();
        }

        swaper_->flush(std::bind(&LogFile::append, &output, std::placeholders::_1, std::placeholders::_2), 
            std::bind(&LogFile::flush, &output));
  }
  output.flush();
}

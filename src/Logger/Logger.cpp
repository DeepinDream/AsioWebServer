#include "Logger.h"
#include "AsyncLogger.h"
#include <assert.h>
#include <iostream>
#include <time.h>  
#include <sys/time.h> 

static std::once_flag flag;
static AsyncLogger *AsyncLogger_;

std::string Logger::logFileName_ = "./WebServer.log";

void once_init()
{
    AsyncLogger_ = new AsyncLogger(Logger::getLogFileName());
    AsyncLogger_->start(); 
}

void output(const char* msg, int len)
{
    // pthread_once(&once_control_, once_init);
    std::call_once(flag, once_init);
    AsyncLogger_->append(msg, len);
}

Logger::Impl::Impl(const char *fileName, int line)
  : stream_(),
    line_(line),
    basename_(fileName)
{
    formatTime();
}

void Logger::Impl::formatTime()
{
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday (&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);   
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    stream_ << str_t;
}

Logger::Logger(const char *fileName, int line)
  : impl_(fileName, line)
{ }

Logger::~Logger()
{
    impl_.stream_ << " -- " << impl_.basename_ << ':' << impl_.line_ << '\n';
    const LogStream::Buffer& buf(impl_.stream_.buffer());
    output(buf.data(), buf.length());
}
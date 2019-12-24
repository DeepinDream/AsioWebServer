#include "Logger.h"
#include "AsyncLogger.h"
#include <assert.h>
#include <iostream>
#include <time.h>  
#include <sys/time.h> 
#include <mutex>
#include "AsyncLogger2.h"
#include "AsyncLogger.h"

static std::once_flag flag;
static std::unique_ptr<AsyncLogger> AsyncLogger_;

std::string Logger::logFileName_ = "./WebServer.log";
LogLevel Logger::logLevel_ = LogLevel::INFO;

static char LEVEL_STR[][4] = {"[E]", "[W]", "[I]"};

void once_init()
{
    AsyncLogger_.reset(new AsyncLogger(Logger::getLogFileName()));
    AsyncLogger_->start(); 
}

void output(const char* msg, int len)
{
    std::call_once(flag, once_init);
    AsyncLogger_->append(msg, len);
}

Logger::Impl::Impl(const char *fileName, int line, LogLevel level)
  : stream_(),
    line_(line),
    basename_(fileName)
{
    stream_ << LEVEL_STR[static_cast<int>(level)];
    formatTime();
    stream_ << "[" << basename_ << ':' << line_ << ']';
}

inline void Logger::Impl::formatTime()
{
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday (&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);   
    strftime(str_t, 26, "[%Y-%m-%d %H:%M:%S]", p_time);
    stream_ << str_t;
}

Logger::Logger(const char *fileName, int line, LogLevel level)
  : impl_(fileName, line, level)
{ }

Logger::~Logger()
{
    impl_.stream_ << '\n';
    const LogStream::Buffer& buf(impl_.stream_.buffer());
    output(buf.data(), buf.length());
}

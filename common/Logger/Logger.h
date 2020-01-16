#pragma once
#include "LogStream.h"
#include <stdio.h>
#include <string>

class AsyncLogger;

enum class LogLevel { ERROR, WARNING, INFO };

class Logger {
  public:
    Logger(const char* fileName, int line, LogLevel level);
    ~Logger();
    LogStream& stream()
    {
        return impl_.stream_;
    }

    static void setLogFileName(std::string fileName)
    {
        logFileName_ = fileName;
    }
    static std::string getLogFileName()
    {
        return logFileName_;
    }

    static void setLogLevel(LogLevel logLevel)
    {
        logLevel_ = logLevel;
    }
    static LogLevel getLogLevel()
    {
        return logLevel_;
    }

    static void setCoutEnabled(bool enabled)
    {
        coutEnabled_ = enabled;
    }
    static bool getCoutEnabled()
    {
        return coutEnabled_;
    }

  private:
    class Impl {
      public:
        Impl(const char* fileName, int line, LogLevel level);
        void formatTime();

        LogStream stream_;
        int line_;
        std::string basename_;
    };
    Impl impl_;
    static std::string logFileName_;
    static LogLevel logLevel_;
    static bool coutEnabled_;
};

#define _LOG_(level) Logger(__PRETTY_FUNCTION__, __LINE__, level).stream()

#define LOG_INFO                                                               \
    if (LogLevel::INFO <= Logger::getLogLevel())                               \
    _LOG_(LogLevel::INFO)
#define LOG_WANRING                                                            \
    if (LogLevel::WARNING <= Logger::getLogLevel())                            \
    _LOG_(LogLevel::WARNING)
#define LOG_ERROR                                                              \
    if (LogLevel::ERROR <= Logger::getLogLevel())                              \
    _LOG_(LogLevel::ERROR)

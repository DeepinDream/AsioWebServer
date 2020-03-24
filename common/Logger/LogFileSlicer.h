#pragma once

#include <string>

class LogFileSlicer {
  public:
    LogFileSlicer(const std::string& filename, const std::size_t MaxSize);

    std::string getLogName();

  private:
    const std::size_t kMaxSize_;
    unsigned int count_;
    std::string basename_;
};
#pragma once
#include <string>
#include <stdio.h>

class AppendFile
{
public:
    explicit AppendFile(std::string filename);
    ~AppendFile();
    // append 会向文件写
    void append(const char *logline, const size_t len);
    void flush();

private:
    AppendFile(const AppendFile &) = delete;
    const AppendFile &operator=(const AppendFile &) = delete;
    size_t write(const char *logline, size_t len);

    FILE *fp_;
    char buffer_[64 * 1024];
};
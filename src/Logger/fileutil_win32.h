#pragma once
#include <string>
#include <fstream>

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
    std::streamoff write(const char *logline, std::streamsize len);

    std::ofstream fs_;
    char buffer_[64 * 1024];
};

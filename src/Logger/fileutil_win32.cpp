#include "fileutil_win32.h"
#include <iostream>

using namespace std;

AppendFile::AppendFile(string filename)
{
    fs_.open(filename);
  // 用户提供缓冲区
    fs_.rdbuf()->pubsetbuf(buffer_, sizeof(buffer_));
}

AppendFile::~AppendFile()
{
    fs_.close();
}

void AppendFile::append(const char* logline, const size_t len) {
  auto n = this->write(logline, len);
  auto remain = len - n;
  while (remain > 0) {
    size_t x = this->write(logline + n, remain);
    if (x == 0) {
      if (fs_.fail())
        std::cerr << "AppendFile::append() failed !\n" << std::endl;
      break;
    }
    n += x;
    remain = len - n;
  }
}

void AppendFile::flush()
{
    fs_.flush();
}

std::streamoff AppendFile::write(const char* logline, std::streamsize len) {
    auto start = fs_.tellp();
    auto end = fs_.write(logline, len).tellp();
    return end - start;
}

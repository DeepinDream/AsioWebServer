#include "fileutil_win32.h"

using namespace std;

AppendFile::AppendFile(string filename)
{
    fs_.open(filename);
  // 用户提供缓冲区
//    setbuf(fp_, buffer_);
    fs_.rdbuf()->pubsetbuf(buffer_, sizeof(buffer_));
}

AppendFile::~AppendFile()
{
//    fclose(fp_);
    fs_.close();
}

void AppendFile::append(const char* logline, const size_t len) {
    fs_.write(logline, len);
}

void AppendFile::flush()
{
//    fflush(fp_);
    fs_.flush();
}

//size_t AppendFile::write(const char* logline, size_t len) {
//    fs_.wr
//}

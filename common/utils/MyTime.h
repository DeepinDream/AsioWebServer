#pragma once

#include <ctime>    // std::time_t
#include <iomanip>  // std::put_time
#include <string>

namespace utils {
inline std::string get_time_str(std::time_t t)
{
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

inline std::string get_gmt_time_str(std::time_t t)
{
    struct tm* GMTime    = gmtime(&t);
    char       buff[512] = {0};
    strftime(buff, sizeof(buff), "%a, %d %b %Y %H:%M:%S %Z", GMTime);
    return buff;
}

inline std::string get_cur_time_str()
{
    return get_time_str(std::time(nullptr));
}
}  // namespace utils

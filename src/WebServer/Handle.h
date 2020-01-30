#pragma once

#include "Response.h"
#include "ServerBase.h"
#include <fstream>

using namespace std;
using namespace AsioWeb;

// 处理访问 /string 的 POST 请求，返回 POST 的字符串
void PostString(Response& response, WebRequest& request)
{
    // 从 istream 中获取字符串 (*request.content)
    stringstream ss;
    string content("empty param!");
    if (request.content.get() != nullptr) {
        *request.content >> ss.rdbuf();  // 将请求内容读取到 stringstream
        content = ss.str();
    }
    response.set_status_and_content(status_type::ok, std::move(content));
}

// 处理访问 /info 的 GET 请求, 返回请求的信息
void GetInfo(Response& response, WebRequest& request)
{
    stringstream content_stream;
    content_stream << "<h1>WebRequest:</h1>";
    content_stream << request.method << " " << request.path << " HTTP/"
                   << request.http_version << "<br>";
    for (auto& header : request.header) {
        content_stream << header.first << ": " << header.second << "<br>";
    }

    std::string content = content_stream.str();
    response.set_status_and_content(status_type::ok, std::move(content));
}

// 处理访问 /match/[字母+数字组成的字符串] 的 GET 请求, 例如执行请求 GET
// /match/abc123, 将返回 abc123
void GetMatch(Response& response, WebRequest& request)
{
    string number = request.path_match[1];

    response.set_status_and_content(status_type::ok, std::move(number));
}

std::string getFilePath(const std::string& file)
{
    string filename = "resource/web/";

    string path = file;

    // 防止使用 `..` 来访问 web/ 目录外的内容
    size_t last_pos = path.rfind(".");
    size_t current_pos = 0;
    size_t pos;
    while ((pos = path.find('.', current_pos)) != string::npos &&
           pos != last_pos) {
        current_pos = pos;
        path.erase(pos, 1);
        last_pos--;
    }

    filename += path;

    return filename;
}

// 处理默认 GET 请求, 如果没有其他匹配成功，则这个函数会被调用
// 将应答 web/ 目录及其子目录中的文件
// 默认文件: index.html
void GetDefault(Response& response, WebRequest& request)
{
    string filename = getFilePath(request.path_match[1]);

    ifstream ifs;
    // 简单的平台无关的文件或目录检查
    if (filename.find('.') == string::npos) {
        if (filename[filename.length() - 1] != '/')
            filename += '/';
        filename += "index.html";
    }
    ifs.open(filename, ifstream::in);

    if (ifs) {
        // 文件内容拷贝到 response-stream 中，不应该用于大型文件
        std::string str((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
        response.set_status_and_content(status_type::ok, std::move(str));

        ifs.close();
    }
    else {
        // 文件不存在时，返回无法打开文件
        string content = "Could not open file " + filename;
        response.set_status_and_content(status_type::not_found,
                                        std::move(content));
    }
}

inline std::string to_hex_string(std::size_t value)
{
    std::ostringstream stream;
    stream << std::hex << value;
    return stream.str();
}

void writeChunkedHeader(Response& response, std::string_view mime)
{
    response.add_header("Access-Control-Allow-origin", "*");
    response.add_header("Content-type", std::string(mime.data(), mime.size()) +
                                            "; charset=utf8");
    response.set_status_and_content(status_type::ok, "",
                                    res_content_type::string);
    response.chunkedData().setEnabled(true);
}

void DownloadFile(Response& response, WebRequest& request)
{
    auto& chunk = response.chunkedData();

    string filename = getFilePath(request.path_match[1]);

    ifstream ifs;
    ifs.open(filename, ifstream::in);
    if (!ifs) {
        // 文件不存在时，返回无法打开文件
        string content = "Could not open file " + filename;
        response.set_status_and_content(status_type::not_found,
                                        std::move(content));
        return;
    }
    auto mime = get_mime({filename.data(), filename.length()});

    static int count = 0;
    switch (chunk.getProcState()) {
        case DataProcState::DATA_BEGIN: {
            writeChunkedHeader(response, mime);
            chunk.setSeekPos(0);
        } break;
        case DataProcState::DATA_CONTINUE: {
            response.clear();
            ifs.seekg(0, std::ios::end);
            size_t size = chunk.k_MaxLength;
            // size_t size = 3 * 1024;
            std::string str;
            str.resize(size);
            ifs.seekg(chunk.getSeekPos());
            ifs.read(&str[0], size);

            size_t read_len = (size_t)ifs.gcount();
            if (read_len != size) {
                str.resize(read_len);
            }
            chunk.addSeekPos(read_len);
            bool eof = (read_len == 0 || read_len != size);
            response.set_chunked_content(std::move(str), eof);
            // if (count < 10) {
            //     std::string str = "chunked data test: " +
            //     std::to_string(count);
            //     // std::cout << str << std::endl;
            //     response.set_chunked_content(std::move(str));
            //     count++;
            // }
            // else {
            //     response.set_chunked_content(std::string(), true);
            //     count = 0;
            // }

        } break;
        case DataProcState::DATA_END:
        case DataProcState::DATA_ALL_END:
        case DataProcState::DATA_CLOSE:
        case DataProcState::DATA_ERR:
            count = 0;
            chunk.setEnabled(false);
            chunk.setProcState(DataProcState::DATA_BEGIN);
            break;
    }

    // ifs.close();
}

void DownloadFileNotChunked(Response& response, WebRequest& request)
{
    response.add_header("Transfer-Encoding", "chunked");
    // std::string str = "chunked data test: 0";
    // std::string length = to_hex_string(str.size());
    // std::string content = length + "\r\n" + str + "\r\n" + "0\r\n\r\n";
    // response.set_status_and_content(status_type::ok, std::move(content),
    //                                 res_content_type::string);

    std::string str = "chunked data test: " + std::to_string(0);
    std::cout << str << std::endl;
    response.set_content(std::move(str));
}
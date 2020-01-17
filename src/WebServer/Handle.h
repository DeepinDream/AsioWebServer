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
        response.set_status_and_content(status_type::ok, std::move(content));
    }
}

void DownloadFile(Response& response, WebRequest& request)
{
    auto chunk = response.chunkedData();

    string filename = getFilePath(request.path_match[1]);

    ifstream ifs;
    ifs.open(filename, ifstream::in);
    if (!ifs) {
        return;
    }

    chunk.setEnabled(true);
    // switch (chunk.getProcState()){

    // }

    ifs.close();
}
#pragma once

#include "ServerBase.h"
#include "HttpAssembler.h"
#include <fstream>

using namespace std;
using namespace AsioWeb;

// 处理访问 /string 的 POST 请求，返回 POST 的字符串
void PostString(ostream& response, WebRequest& request)
{
	// 从 istream 中获取字符串 (*request.content)
	stringstream ss;
	string content("empty param!");
	if (request.content.get() != nullptr)
	{
		*request.content >> ss.rdbuf(); // 将请求内容读取到 stringstream
		content = ss.str();
	}

	// 直接返回请求结果
	response << getResponse(content);
}

// 处理访问 /info 的 GET 请求, 返回请求的信息
void GetInfo(ostream &response, WebRequest &request)
{
	stringstream content_stream;
	content_stream << "<h1>WebRequest:</h1>";
	content_stream << request.method << " " << request.path << " HTTP/" << request.http_version << "<br>";
	for (auto &header : request.header)
	{
		content_stream << header.first << ": " << header.second << "<br>";
	}

	// 获得 content_stream 的长度(使用 content.tellp() 获得)
	content_stream.seekp(0, ios::end);

	response << "HTTP/1.1 200 OK\r\nContent-Length: " << content_stream.tellp() << "\r\n\r\n"
			 << content_stream.rdbuf();
	// auto res = getResponse(content_stream);
	// response << res;
}

// 处理访问 /match/[字母+数字组成的字符串] 的 GET 请求, 例如执行请求 GET /match/abc123, 将返回 abc123
void GetMatch(ostream &response, WebRequest &request)
{
	string number = request.path_match[1];
	response << "HTTP/1.1 200 OK\r\nContent-Length: " << number.length() << "\r\n\r\n"
			 << number;
}

// 处理默认 GET 请求, 如果没有其他匹配成功，则这个函数会被调用
// 将应答 web/ 目录及其子目录中的文件
// 默认文件: index.html
void GetDefault(ostream &response, WebRequest &request)
{
	string filename = "resource/web/";

	string path = request.path_match[1];

	// 防止使用 `..` 来访问 web/ 目录外的内容
	size_t last_pos = path.rfind(".");
	size_t current_pos = 0;
	size_t pos;
	while ((pos = path.find('.', current_pos)) != string::npos && pos != last_pos)
	{
		current_pos = pos;
		path.erase(pos, 1);
		last_pos--;
	}

	filename += path;
	ifstream ifs;
	// 简单的平台无关的文件或目录检查
	if (filename.find('.') == string::npos)
	{
		if (filename[filename.length() - 1] != '/')
			filename += '/';
		filename += "index.html";
	}
	ifs.open(filename, ifstream::in);

	if (ifs)
	{
		ifs.seekg(0, ios::end);
		size_t length = ifs.tellg();

		ifs.seekg(0, ios::beg);

		// 文件内容拷贝到 response-stream 中，不应该用于大型文件
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << length << "\r\n\r\n"
				 << ifs.rdbuf();

		ifs.close();
	}
	else
	{
		// 文件不存在时，返回无法打开文件
		string content = "Could not open file " + filename;
		response << "HTTP/1.1 400 Bad WebRequest\r\nContent-Length: " << content.length() << "\r\n\r\n"
				 << content;
	}
}
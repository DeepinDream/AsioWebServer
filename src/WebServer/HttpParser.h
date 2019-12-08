#pragma once
#include "WebRequest.h"

namespace AsioWeb
{
	// 解析请求
	WebRequest parse_request(std::istream& stream)  {
		WebRequest request;

		// 使用正则表达式对请求报头进行解析，通过下面的正则表达式
		// 可以解析出请求方法(GET/POST)、请求路径以及 HTTP 版本
		std::regex e("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");

		std::smatch sub_match;

		//从第一行中解析请求方法、路径和 HTTP 版本
		std::string line;
		getline(stream, line);
		line.pop_back();
		if (std::regex_match(line, sub_match, e)) {
			request.method = sub_match[1];
			request.path = sub_match[2];
			request.http_version = sub_match[3];

			// 解析头部的其他信息
			bool matched;
			e = "^([^:]*): ?(.*)$";
			do {
				getline(stream, line);
				line.pop_back();
				matched = std::regex_match(line, sub_match, e);
				if (matched) {
					request.header[sub_match[1]] = sub_match[2];
				}
			} while (matched == true);
		}
		return request;
	}
}
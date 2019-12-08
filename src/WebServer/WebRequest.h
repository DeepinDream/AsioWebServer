#pragma once

#include <string>
#include <memory>
#include <iostream>		// std::istream
#include <unordered_map>	// std::unordered_map
#include <regex>	// std::smatch
#include <map>
#include <functional>

namespace AsioWeb
{
	struct WebRequest
	{
		// 请求方法, POST, GET; 请求路径; HTTP 版本
		std::string method, path, http_version;
		// 对 content 使用智能指针进行引用计数
		std::shared_ptr<std::istream> content;
		// 哈希容器, key-value 字典
		std::unordered_map<std::string, std::string> header;
		// 用正则表达式处理路径匹配
		std::smatch path_match;
	};

	using RequstPathRegx = std::string;
	using RequstType = std::string;
	using RequestFunc = std::function<void(std::ostream&, WebRequest&)>;

	using ResourceType = typename std::map<RequstPathRegx, std::unordered_map<RequstType, RequestFunc>>;
}

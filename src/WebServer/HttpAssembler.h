#pragma once

#include <string>
#include <iostream>

#include "utils/MyString.h"

namespace AsioWeb
{
    // 封装答复
    std::string getResponse(const std::string& content)
    {
        return utils::StringFormat(std::string("HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%s"), content.length(), content);
    }

    // 封装答复
    auto getResponse(std::stringstream& contentStream) -> decltype(contentStream.rdbuf())
    {
        std::stringstream response;
	    response << "HTTP/1.1 200 OK\r\nContent-Length: " << contentStream.tellp() << "\r\n\r\n"
			 << contentStream.rdbuf();
        return response.rdbuf();
    }

    void HttpPrint(std::ostream& os, const std::string& content)
    {
        os << "HTTP/1.1 200 OK\r\nContent-Length: " << content.size() << "\r\n\r\n"
           << content;
    }

    void HttpPrint(std::ostream& os, std::stringstream& contentStream)
    {
        os << "HTTP/1.1 200 OK\r\nContent-Length: " << contentStream.tellp() << "\r\n\r\n"
           << contentStream.rdbuf();
    }

}
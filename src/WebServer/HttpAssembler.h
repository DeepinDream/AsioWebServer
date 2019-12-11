#pragma once

#include <string>
#include <iostream>

#include "utils/MyString.h"

namespace AsioWeb
{
    static const std::string HTTP_HEAD("HTTP/1.1 200 OK\r\n");

    void HttpPrint(std::ostream& os, const std::string& content)
    {
        os << HTTP_HEAD << "Content-Length: " << content.size() << "\r\n\r\n"
           << content;
    }

    void HttpPrint(std::ostream& os, std::stringstream& contentStream)
    {
        os << "HTTP/1.1 200 OK\r\nContent-Length: " << contentStream.tellp() << "\r\n\r\n"
           << contentStream.rdbuf();
    }

}
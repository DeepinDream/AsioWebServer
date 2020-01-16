#pragma once

#include "UseAsio.h"
#include "mime_types.hpp"
#include "response_cv.hpp"
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace AsioWeb {

class Response {
  public:
    std::vector<boost::asio::const_buffer>
    get_response_buffer(std::string&& body)
    {
        set_content(std::move(body));

        auto buffers = to_buffers();
        return buffers;
    }
    std::vector<boost::asio::const_buffer> to_buffers()
    {
        std::vector<boost::asio::const_buffer> buffers;
        add_header("Host", "AsioWeb");

        buffers.reserve(headers_.size() * 4 + 5);
        buffers.emplace_back(to_buffer(status_));
        for (auto const& h : headers_) {
            buffers.emplace_back(boost::asio::buffer(h.first));
            buffers.emplace_back(boost::asio::buffer(name_value_separator));
            buffers.emplace_back(boost::asio::buffer(h.second));
            buffers.emplace_back(boost::asio::buffer(crlf));
        }

        buffers.push_back(boost::asio::buffer(crlf));

        buffers.emplace_back(
            boost::asio::buffer(content_.data(), content_.size()));

        return buffers;
    }

    void add_header(std::string&& key, std::string&& value)
    {
        headers_[std::move(key)] = std::move(value);
    }

    void set_status(status_type status)
    {
        status_ = status;
    }

    status_type get_status() const
    {
        return status_;
    }

    void set_content(std::string&& content)
    {
        add_header("Content-Length", std::to_string(content.size()));
        content_ = std::move(content);
    }

    void set_status_and_content(status_type status)
    {
        status_ = status;
        set_content(to_string(status).data());
    }
    void
    set_status_and_content(status_type status,
                           std::string&& content,
                           res_content_type res_type = res_content_type::none)
    {
        status_ = status;
        if (res_type != res_content_type::none) {
            auto iter = res_mime_map.find(res_type);
            if (iter != res_mime_map.end()) {
                add_header("Content-type", std::string(iter->second.data(),
                                                       iter->second.size()));
            }
        }

        set_content(std::move(content));
    }

  private:
    std::unordered_map<std::string, std::string> headers_;
    std::string content_;
    status_type status_ = status_type::init;
};
}  // namespace AsioWeb
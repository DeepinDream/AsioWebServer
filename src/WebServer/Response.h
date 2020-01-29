#pragma once

#include "ChunkedData.h"
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
        add_header("Content-Length", std::to_string(content_.size()));

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

    std::vector<boost::asio::const_buffer> to_chunked_buffers()
    {
        if (chunked_data_.getProcState() == DataProcState::DATA_BEGIN) {
            chunked_data_.setProcState(DataProcState::DATA_CONTINUE);
            return to_chunked_headers();
        }

        if (chunked_data_.getProcState() == DataProcState::DATA_CONTINUE) {
            if (chunked_data_.getFinished()) {
                chunked_data_.setProcState(DataProcState::DATA_END);
            }
            // return std::vector<boost::asio::const_buffer>();
            return to_chunked_body(content_, chunked_data_.getFinished());
            // return to_chunked_body(get_chunked_content(content_),
            //                        chunked_data_.getFinished());
        }

        return std::vector<boost::asio::const_buffer>();
    }

    std::vector<boost::asio::const_buffer> to_chunked_headers()
    {
        std::vector<boost::asio::const_buffer> buffers;
        add_header("Transfer-Encoding", "chunked");
        // add_header("Connection", "keep-alive");
        // add_header("Accept-Ranges", "bytes");
        // add_header("Access-Control-Allow-origin", "*");
        // buffers.reserve(headers_.size() * 4 + 5);
        buffers.push_back(to_buffer(status_));
        for (auto const& h : headers_) {
            buffers.push_back(boost::asio::buffer(h.first));
            buffers.push_back(boost::asio::buffer(name_value_separator));
            buffers.push_back(boost::asio::buffer(h.second));
            buffers.push_back(boost::asio::buffer(crlf));
        }

        buffers.push_back(boost::asio::buffer(crlf));
        return buffers;
    }

    std::vector<boost::asio::const_buffer>
    to_chunked_body(const std::string& chunk_data, bool eof)
    {
        std::vector<boost::asio::const_buffer> buffers;

        if (chunk_data.size() > 0) {
            // convert bytes transferred count to a hex string.
            std::string chunk_size = to_hex_string(chunk_data.size());

            // Construct chunk based on rfc2616 section 3.6.1
            // buffers.push_back(boost::asio::buffer(chunk_size,
            // chunk_size.size()));
            // buffers.push_back(boost::asio::buffer(crlf));
            // buffers.push_back(boost::asio::buffer(chunk_data,
            // chunk_data.size()));
            // buffers.push_back(boost::asio::buffer(crlf));

            // std::string content = chunk_size + "\r\n" + content_ + "\r\n";
            // std::cout << content << std::endl;
            // buffers.push_back(boost::asio::buffer(content));

            std::cout << chunk_data << std::endl;
            buffers.push_back(boost::asio::buffer(chunk_data));
        }

        // append last-chunk
        if (eof) {
            buffers.push_back(boost::asio::buffer(last_chunk));
            buffers.push_back(boost::asio::buffer(crlf));
        }

        return buffers;
    }

    void add_header(std::string&& key, std::string&& value)
    {
        headers_[std::move(key)] = std::move(value);
    }

    void clear()
    {
        headers_.clear();
        content_.clear();
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
        content_ = std::move(content);
    }

    void set_chunked_content(std::string&& content, const bool eof = false)
    {
        if (eof == false && content.size() > 0) {
            content_ = get_chunked_content(std::move(content));
        }

        // if (eof) {
        chunked_data_.setFinished(eof);
        // }
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

    ChunkedData& chunkedData()
    {
        return chunked_data_;
    }

    std::string get_chunked_content(const std::string& str)
    {
        std::string length = to_hex_string(str.size());
        std::string content = length + crlf + str + crlf;
        return content;
    }

    std::string to_hex_string(std::size_t value)
    {
        std::ostringstream stream;
        stream << std::hex << value;
        return stream.str();
    }

  private:
    std::unordered_map<std::string, std::string> headers_;
    std::string content_;
    status_type status_ = status_type::init;
    ChunkedData chunked_data_;
};
}  // namespace AsioWeb
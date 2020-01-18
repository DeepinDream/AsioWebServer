#pragma once

#include "UseAsio.h"
#include "WebRequest.h"
#include <memory>

namespace AsioWeb {
// socket_type 为 HTTP or HTTPS
template <typename socket_type>
class Connection
    : public std::enable_shared_from_this<Connection<socket_type>> {
  public:
    explicit Connection(std::shared_ptr<socket_type> socket,
                        std::vector<ResourceType::iterator>& resources)
        : socket_(socket)
        , all_resources(resources)
    {
    }

    // 处理请求和应答
    void process_request_and_respond()
    {
        // 为 async_read_untile() 创建新的读缓存
        auto read_buffer = std::make_shared<boost::asio::streambuf>();
        auto self = this->shared_from_this();
        response_ = Response();
        request_ = WebRequest();
        boost::asio::async_read_until(
            *socket_, *read_buffer, "\r\n\r\n",
            [this, self, read_buffer](const boost::system::error_code& ec,
                                      size_t bytes_transferred) {
                if (!ec) {
                    // 注意：read_buffer->size() 的大小并一定和
                    // bytes_transferred 相等， Boost 的文档中指出： 在
                    // async_read_until 操作成功后,  streambuf
                    // 在界定符之外可能包含一些额外的的数据
                    // 所以较好的做法是直接从流中提取并解析当前 read_buffer
                    // 左边的报头, 再拼接 async_read 后面的内容
                    size_t total = read_buffer->size();

                    // 转换到 istream
                    std::istream stream(read_buffer.get());
                    // std::string s(std::istreambuf_iterator<char>(stream),
                    // {});

                    // 接下来要将 stream 中的请求信息进行解析，然后保存到
                    // request 对象中
                    request_ = parse_request(stream);

                    size_t num_additional_bytes = total - bytes_transferred;

                    // 如果满足，同样读取
                    if (request_.header.count("Content-Length") > 0) {
                        boost::asio::async_read(
                            *socket_, *read_buffer,
                            boost::asio::transfer_exactly(
                                stoull(request_.header["Content-Length"]) -
                                num_additional_bytes),
                            [this, self,
                             read_buffer](const boost::system::error_code& ec,
                                          size_t bytes_transferred) {
                                if (!ec) {
                                    // 将指针作为 istream 对象存储到 read_buffer
                                    // 中
                                    request_.content =
                                        std::shared_ptr<std::istream>(
                                            new std::istream(
                                                read_buffer.get()));
                                    this->respond(response_, request_);
                                }
                            });
                    }
                    else {
                        this->respond(response_, request_);
                    }
                }
                else {
                    std::cout << "server::handle_accept: " << ec.message();
                }
            });
    }

    // 应答
    void respond(Response& response, WebRequest& request)
    {
        // 对请求路径和方法进行匹配查找，并生成响应
        for (auto res_it : all_resources) {
            std::regex e(res_it->first);
            std::smatch sm_res;
            if (std::regex_match(request.path, sm_res, e)) {
                if (res_it->second.count(request.method) > 0) {
                    request.path_match = move(sm_res);

                    handler_ = res_it->second[request.method];

                    Handler(response, request);

                    return;
                }
            }
        }
    }

    void Handler(Response& res, WebRequest& request)
    {
        // web 响应处理
        handler_(res, request);

        // 在 lambda 中捕获 write_buffer 使其不会再 async_write
        // 完成前被销毁

        if (res.chunkedData().getEnabled()) {
            auto write_buffer = res.to_chunked_buffers();
            boost::asio::async_write(
                *socket_, write_buffer,
                [&, self = this->shared_from_this(),
                 this](const boost::system::error_code& ec,
                       size_t bytes_transferred) {
                    // HTTP 持久连接(HTTP 1.1), 递归调用

                    if (!ec && stof(request.http_version) > 1.05) {
                        if (res.chunkedData().getProcState() >=
                            DataProcState::DATA_END) {
                            process_request_and_respond();
                        }
                        else {
                            self->Handler(response_, request_);
                        }
                    }

                    if (ec) {
                        std::cout << "server::handle_accept: " << ec.message()
                                  << std::endl;
                    }
                });
        }
        else {
            auto write_buffer = res.to_buffers();
            boost::asio::async_write(
                *socket_, write_buffer,
                [this, self = this->shared_from_this(), request,
                 write_buffer](const boost::system::error_code& ec,
                               size_t bytes_transferred) {
                    // HTTP 持久连接(HTTP 1.1), 递归调用
                    if (!ec && stof(request.http_version) > 1.05)
                        process_request_and_respond();
                });
        }
    }

  private:
    std::shared_ptr<socket_type> socket_;
    std::vector<ResourceType::iterator> all_resources;
    RequestFunc handler_;
    WebRequest request_;
    Response response_;
};
}  // namespace AsioWeb
#pragma once

#include "HttpParser.h"
#include "IOServicePool.h"
#include "Response.h"
#include "UseAsio.h"
#include "WebRequest.h"
#include <iostream>
#include <memory>
#include <vector>

namespace AsioWeb {
// socket_type 为 HTTP or HTTPS
template <typename socket_type>
class ServerBase {
  public:
    // 构造服务器, 初始化端口, 默认使用一个线程
    explicit ServerBase(unsigned short port, size_t num_threads = 1)
        : m_io_service(num_threads)
        , endpoint(boost::asio::ip::tcp::v4(), port)
        , acceptor(m_io_service.getIOService(), endpoint)
    // , strand_(m_io_service.getIOService())
    {
    }

    virtual ~ServerBase() {}

    // 启动服务器
    void start()
    {
        std::cout << __FUNCTION__ << std::endl;
        // 默认资源放在 vector 的末尾, 用作默认应答
        // 默认的请求会在找不到匹配请求路径时，进行访问，故在最后添加
        for (auto it = resource.begin(); it != resource.end(); it++) {
            all_resources.push_back(it);
        }
        for (auto it = default_resource.begin(); it != default_resource.end();
             it++) {
            all_resources.push_back(it);
        }

        // 调用 socket 的连接方式，还需要子类来实现 accept() 逻辑
        accept();

        m_io_service.run();
    }

    ResourceType resource;
    ResourceType default_resource;

  protected:
    // 需要不同类型的服务器实现这个方法
    virtual void accept() {}
    // 处理请求和应答
    void process_request_and_respond(std::shared_ptr<socket_type> socket) const
    {
        // 为 async_read_untile() 创建新的读缓存
        auto read_buffer = std::make_shared<boost::asio::streambuf>();

        boost::asio::async_read_until(
            *socket, *read_buffer, "\r\n\r\n",
            [this, socket, read_buffer](const boost::system::error_code& ec,
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

                    // 被推导为 std::shared_ptr<WebRequest> 类型
                    auto request = std::make_shared<WebRequest>();

                    // 接下来要将 stream 中的请求信息进行解析，然后保存到
                    // request 对象中
                    *request = parse_request(stream);

                    size_t num_additional_bytes = total - bytes_transferred;

                    // 如果满足，同样读取
                    if (request->header.count("Content-Length") > 0) {
                        boost::asio::async_read(
                            *socket, *read_buffer,
                            boost::asio::transfer_exactly(
                                stoull(request->header["Content-Length"]) -
                                num_additional_bytes),
                            [this, socket, read_buffer,
                             request](const boost::system::error_code& ec,
                                      size_t bytes_transferred) {
                                if (!ec) {
                                    // 将指针作为 istream 对象存储到 read_buffer
                                    // 中
                                    request->content =
                                        std::shared_ptr<std::istream>(
                                            new std::istream(
                                                read_buffer.get()));
                                    respond(socket, request);
                                }
                            });
                    }
                    else {
                        respond(socket, request);
                    }
                }
            });
    }

    // 应答
    void respond(std::shared_ptr<socket_type> socket,
                 std::shared_ptr<WebRequest> request) const
    {
        // 对请求路径和方法进行匹配查找，并生成响应
        for (auto res_it : all_resources) {
            std::regex e(res_it->first);
            std::smatch sm_res;
            if (std::regex_match(request->path, sm_res, e)) {
                if (res_it->second.count(request->method) > 0) {
                    request->path_match = move(sm_res);

                    // 会被推导为 std::shared_ptr<boost::asio::streambuf>
                    Response response;
                    res_it->second[request->method](response, *request);

                    // 在 lambda 中捕获 write_buffer 使其不会再 async_write
                    // 完成前被销毁
                    auto write_buffer = response.to_buffers();
                    boost::asio::async_write(
                        *socket, write_buffer,
                        [this, socket, request,
                         write_buffer](const boost::system::error_code& ec,
                                       size_t bytes_transferred) {
                            // HTTP 持久连接(HTTP 1.1), 递归调用
                            if (!ec && stof(request->http_version) > 1.05)
                                process_request_and_respond(socket);
                        });
                    return;
                }
            }
        }
    }

    // asio 库中的 io_service 是调度器，所有的异步 IO 事件都要通过它来分发处理
    // 换句话说, 需要 IO 的对象的构造函数，都需要传入一个 io_service 对象
    IOServicePool m_io_service;
    // IP 地址、端口号、协议版本构成一个 endpoint，并通过这个 endpoint
    // 在服务端生成 tcp::acceptor 对象，并在指定端口上等待连接
    boost::asio::ip::tcp::endpoint endpoint;
    // 所以，一个 acceptor 对象的构造都需要 io_service 和 endpoint 两个参数
    boost::asio::ip::tcp::acceptor acceptor;

    // // 如果多个 event handler 通过同一个 strand 对象分发 (dispatch)，那么这些
    // // event handler 就会保证顺序地执行。
    // // 使用 strand，所以不需要使用互斥锁保证同步了
    // boost::asio::io_service::strand strand_;

    // 所有的资源及默认资源都会在 vector 尾部添加, 并在 start() 中创建
    std::vector<ResourceType::iterator> all_resources;
};

template <typename socket_type>
class Server : public ServerBase<socket_type> {
};

}  // namespace AsioWeb
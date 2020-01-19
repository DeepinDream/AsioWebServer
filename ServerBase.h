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
#pragma once

#include "Connection.h"
#include "ServerBase.h"
#include "UseAsio.h"

namespace AsioWeb {
using HTTP = boost::asio::ip::tcp::socket;
template <>
class Server<HTTP> : public ServerBase<HTTP> {
  public:
    // 通过端口号、线程数来构造 Web 服务器, HTTP
    // 服务器比较简单，不需要做相关配置文件的初始化
    Server(unsigned short port, size_t num_threads = 1)
        : ServerBase<HTTP>::ServerBase(port, num_threads){};

  private:
    // 实现 accept() 方法
    void accept()
    {
        // 为当前连接创建一个新的 socket
        // Shared_ptr 用于传递临时对象给匿名函数
        // socket 会被推导为 std::shared_ptr<HTTP> 类型
        auto socket = std::make_shared<HTTP>(m_io_service.getIOService());
        auto connection =
            std::make_shared<Connection<HTTP>>(socket, all_resources);
        acceptor.async_accept(
            *socket,
            [this, connection, socket](const boost::system::error_code& ec) {
                socket->set_option(boost::asio::ip::tcp::no_delay(true));
                // 立即启动并接受一个连接
                accept();

                // 如果出现错误
                if (!ec)
                    connection->process_request_and_respond();
            });
    }
};
}  // namespace AsioWeb
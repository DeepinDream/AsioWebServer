#pragma once

#include "Connection.h"
#include "ServerBase.h"
#include "UseAsio.h"
#include <asio/ssl.hpp>
#include <string>
#include <type_traits>

namespace AsioWeb {
using HTTPS = typename boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
template <>
class Server<HTTPS> : public ServerBase<HTTPS> {
  public:
    // 一个 HTTPS 的服务器比 HTTP
    // 服务器多增加了两个参数，一个是证书文件，另一个是私钥文件
    Server(unsigned short port, size_t num_threads)
        : ServerBase<HTTPS>::ServerBase(port, num_threads)
        , context(boost::asio::ssl::context::sslv23)
    {
    }

    template <typename F>
    void init_ssl_context(const bool ssl_enable_v3,
                          const std::string& cert_file,
                          const std::string& private_key_file,
                          const std::string& tmp_dh_file = std::string(),
                          F&& f = nullptr)
    {
        unsigned long ssl_options =
            boost::asio::ssl::context::default_workarounds |
            boost::asio::ssl::context::no_sslv2 |
            boost::asio::ssl::context::single_dh_use;

        if (!ssl_enable_v3)
            ssl_options |= boost::asio::ssl::context::no_sslv3;

        set_password_callback(std::forward<F>(f));

        context.set_options(ssl_options);
        // 使用证书文件
        context.use_certificate_chain_file(cert_file);
        // 使私钥文件, 相比之下需要多传入一个参数来指明文件的格式
        context.use_private_key_file(private_key_file,
                                     boost::asio::ssl::context::pem);
        if (!tmp_dh_file.empty())
            context.use_tmp_dh_file(std::move(tmp_dh_file));
    }

  private:
    // 和 HTTP 服务器相比，需要多定义一个 ssl context 对象
    boost::asio::ssl::context context;

    template <typename F>
    typename std::enable_if<!std::is_null_pointer<F>::value>::type
    set_password_callback(F&& f)
    {
        context.set_password_callback(std::forward<F>(f));
    }

    template <typename F>
    typename std::enable_if<std::is_null_pointer<F>::value>::type
    set_password_callback(F&& f)
    {
    }

    // HTTPS 服务器和 HTTP 服务器相比
    // 其区别在于对 socket 对象的构造方式有所不同
    // HTTPS 会在 socket 这一步中对 IO 流进行加密
    // 因此实现的 accept() 方法需要对 socket 用 ssl context 初始化
    void accept()
    {
        // 为当前连接创建一个新的 socket
        // Shared_ptr 用于传递临时对象给匿名函数
        // socket 类型会被推导为: std::shared_ptr<HTTPS>
        auto socket =
            std::make_shared<HTTPS>(m_io_service.getIOService(), context);
        auto connection =
            std::make_shared<Connection<HTTPS>>(socket, all_resources);
        acceptor.async_accept(
            (*socket).lowest_layer(),
            [this, socket, connection](const boost::system::error_code& ec) {
                // 立即启动并接受一个新连接
                accept();

                // 处理错误
                if (!ec) {
                    (*socket).async_handshake(
                        boost::asio::ssl::stream_base::server,
                        [this,
                         connection](const boost::system::error_code& ec) {
                            if (!ec)
                                connection->process_request_and_respond();
                        });
                }
            });
    }
};
}  // namespace AsioWeb
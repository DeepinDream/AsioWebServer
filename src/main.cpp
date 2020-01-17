#include <iostream>

#include "WebServer/Handle.h"
#include "WebServer/RestfulApi.h"
#include "WebServer/ServerHttp.h"
#include "WebServer/ServerHttps.h"

#include "Logger/Logger.h"

using namespace AsioWeb;

int main(int, char**)
{
    std::cout << "Hello, world!\n";
    Logger::setCoutEnabled(true);
    LOG_INFO << "Hello world!";

    // // HTTP 服务运行在 12345 端口，并启用四个线程
    auto serverPtr = std::make_shared<Server<HTTP>>(12345, 4);

    // HTTPS 服务运行在 12345 端口，并启用四个线程
    // auto serverPtr = std::make_shared<Server<HTTPS>>(12345, 4);
    // serverPtr->init_ssl_context(
    //     true, "resource/ssl/server_2.crt", "resource/ssl/server_2.key",
    //     "resource/ssl/dh1024.pem",
    //     [](auto, auto) { return std::string("123456"); });

    RestfulApi api(serverPtr->resource);
    api.Post(std::string("^/string/?$"), PostString);
    api.Get(std::string("^/info/?$"), GetInfo);
    api.Get(std::string("^/match/([0-9a-zA-Z]+)/?$"), GetMatch);

    RestfulApi defaultApi(serverPtr->default_resource);
    defaultApi.Get(std::string("^/?(.*)$"), GetDefault);

    serverPtr->start();

    return 0;
}

#include <iostream>

#include "WebServer/ServerHttp.h"
#include "WebServer/ServerHttps.h"
#include "WebServer/Handle.h"
#include "WebServer/RestfulApi.h"

using namespace AsioWeb;

int main(int, char**)
{
    std::cout << "Hello, world!\n";

	// HTTP 服务运行在 12345 端口，并启用四个线程
	auto serverPtr = std::make_shared<Server<HTTP>>(12345, 4);

	// start_server<Server<HTTP>>(*serverPtr);

	RestfulApi<HTTP> api(serverPtr);
	api.Post(std::string("^/string/?$"), PathString);

	serverPtr->start();
	return 0;
}

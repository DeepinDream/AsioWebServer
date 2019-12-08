#pragma once
#include "ServerBase.h"
#include "WebRequest.h"
#include "IResetful.h"
#include <memory>

namespace AsioWeb
{
// socket_type 为 HTTP or HTTPS
template <typename socket_type>
class RestfulApi: IResetful
{
public:
    using ApiCallback = std::function<void(ostream& response, WebRequest& request)>;
    using ServerType = ServerBase<socket_type>;
    using ServerPtr = typename std::shared_ptr<ServerType>;
    explicit RestfulApi(ServerPtr server)    
     : m_server(server)
    {

    }
    
    virtual void Post(const std::string& url, ApiCallback func) override
    {
        m_server->resource[url]["POST"] = func;
    }

    virtual void Get(const std::string& url, ApiCallback func) override
    {
        m_server->resource[url]["POST"] = func;
    }
    virtual void Put(const std::string& url, ApiCallback func) override
    {
        m_server->resource[url]["PUT"] = func;
    }
    virtual void Patch(const std::string& url, ApiCallback func) override
    {
        m_server->resource[url]["PATCH"] = func;
    }
    virtual void Delete(const std::string& url, ApiCallback func) override
    {
        m_server->resource[url]["DELETE"] = func;
    }

private:
    ServerPtr m_server;
};
} // namespace AsioWeb
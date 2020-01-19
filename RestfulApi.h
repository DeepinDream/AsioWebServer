#pragma once
#include "IResetful.h"
#include "ServerBase.h"
#include "WebRequest.h"
#include <memory>

namespace AsioWeb {
// socket_type 为 HTTP or HTTPS
// template <typename socket_type>
class RestfulApi : IResetful {
  public:
    using ApiCallback = RequestFunc;

    explicit RestfulApi(ResourceType& resource)
        : m_resource(resource)
    {
    }

    virtual void Post(const std::string& url, ApiCallback func) override
    {
        // m_server->resource[url]["POST"] = func;
        m_resource[url]["POST"] = func;
    }

    virtual void Get(const std::string& url, ApiCallback func) override
    {
        // m_server->resource[url]["GET"] = func;
        m_resource[url]["GET"] = func;
    }
    virtual void Put(const std::string& url, ApiCallback func) override
    {
        // m_server->resource[url]["PUT"] = func;
        m_resource[url]["PUT"] = func;
    }
    virtual void Patch(const std::string& url, ApiCallback func) override
    {
        // m_server->resource[url]["PATCH"] = func;
        m_resource[url]["PATCH"] = func;
    }
    virtual void Delete(const std::string& url, ApiCallback func) override
    {
        // m_server->resource[url]["DELETE"] = func;
        m_resource[url]["DELETE"] = func;
    }

  private:
    // ServerPtr m_server;
    ResourceType& m_resource;
};
}  // namespace AsioWeb
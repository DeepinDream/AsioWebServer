#pragma once

#include <functional>
#include <iostream>
#include <string>

namespace AsioWeb {

class WebRequest;
class Response;

class IResetful {
  public:
    using ApiCallback =
        std::function<void(Response& response, WebRequest& request)>;

    virtual void Post(const std::string& route, ApiCallback func) = 0;
    virtual void Get(const std::string& route, ApiCallback func) = 0;
    virtual void Put(const std::string& route, ApiCallback func) = 0;
    virtual void Patch(const std::string& route, ApiCallback func) = 0;
    virtual void Delete(const std::string& route, ApiCallback func) = 0;
};
}  // namespace AsioWeb
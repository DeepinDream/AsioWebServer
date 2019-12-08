#pragma once

#include <iostream>
#include <functional>
#include <string>

namespace AsioWeb
{

class WebRequest;

class IResetful
{
public:
    using ApiCallback = std::function<void(std::ostream& response, WebRequest& request)>;
    
    virtual void Post(const std::string& url, ApiCallback func) = 0;
    virtual void Get(const std::string& url, ApiCallback func) = 0;
    virtual void Put(const std::string& url, ApiCallback func) = 0;
    virtual void Patch(const std::string& url, ApiCallback func) = 0;
    virtual void Delete(const std::string& url, ApiCallback func) = 0;
};
} // namespace AsioWeb
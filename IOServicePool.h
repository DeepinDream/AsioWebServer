#pragma once

#include "UseAsio.h"
#include <thread>

namespace AsioWeb {
class IOServicePool {
  public:
    IOServicePool(const size_t threadNum = std::thread::hardware_concurrency())
        : work_(new boost::asio::io_service::work(service_))
        , thread_num_(threadNum)
    {
    }

    boost::asio::io_service& getIOService()
    {
        return service_;
    }
    void run()
    {
        thread_num_ = thread_num_ < 1 ? 1 : thread_num_;
        for (size_t i = 0; i < thread_num_; ++i) {
            threads_.emplace_back([this]() { service_.run(); });
        }
        for (auto& t : threads_) {
            t.join();
        }
    }

  private:
    IOServicePool(const IOServicePool&) = delete;
    IOServicePool& operator=(const IOServicePool&) = delete;
    boost::asio::io_service service_;
    std::unique_ptr<boost::asio::io_service::work> work_;
    std::vector<std::thread> threads_;
    size_t thread_num_;
};
}  // namespace AsioWeb

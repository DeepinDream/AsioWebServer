#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
//#include <iostream>

class ThreadGuard
{
public:
    explicit ThreadGuard(std::function<void()> func)
        : thread_(new std::thread([&](){
//        std::cout << "thread wait start" << std::endl;
        func();
        std::unique_lock<std::mutex> lock(mutex_);
        launched_cv_.notify_all();
    }))
    {

    }
    ~ThreadGuard()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        launched_cv_.wait(lock);
//        std::cout << "thread quit" << std::endl;
    }

    void detach()
    {
        thread_->detach();
//        std::cout << "thread detach" << std::endl;
    }

private:
    ThreadGuard(const ThreadGuard &) = delete;
    const ThreadGuard &operator=(const ThreadGuard &) = delete;

private:
    std::unique_ptr<std::thread> thread_;
    std::condition_variable launched_cv_;
    std::mutex mutex_;
};

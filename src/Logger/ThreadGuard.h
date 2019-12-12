#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class Launcher
{
public:
    Launcher(): is_launch_(false){}

    void setup() {is_launch_.exchange(true);}
    void waitForLaunch(){while(!is_launch_){}}
private:
    std::atomic<bool> is_launch_;
};

class Exiter
{
public:
    void setup()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        exit_cv_.notify_all();
    }
    void waitForStop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        exit_cv_.wait(lock);
    }
private:
    std::mutex mutex_;
    std::condition_variable exit_cv_;
};

class ThreadGuard
{
public:
    explicit ThreadGuard(std::function<void()> func)
        : thread_(new std::thread([&](){
        launcher_.setup();
        func();
        exiter_.setup();
    }))
    {

    }
    ~ThreadGuard()
    {

    }

    void detach()
    {
        thread_->detach();
        launcher_.waitForLaunch();
    }

    void waitForStop()
    {
        exiter_.waitForStop();
    }

private:
    ThreadGuard(const ThreadGuard &) = delete;
    const ThreadGuard &operator=(const ThreadGuard &) = delete;

private:
    std::unique_ptr<std::thread> thread_;
    Launcher launcher_;
    Exiter exiter_;
};

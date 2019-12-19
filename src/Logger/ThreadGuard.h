#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class Launcher
{
public:
    Launcher(): is_launched_(false){}

    void setup() {is_launched_.exchange(true);}
    void wait(){while(!is_launched_){}}
private:
    std::atomic<bool> is_launched_;
};

class Exiter
{
public:
    Exiter(): is_exited_(false){}
    void setup()
    {
        is_exited_.exchange(true);
    }
    void waitForStop()
    {
        while(!is_exited_){}
    }
private:
    std::atomic<bool> is_exited_;
};

class ThreadGuard
{
public:
    explicit ThreadGuard(std::function<void()> func)
        : func_(func)
//        , thread_(new std::thread([&](){
//                launcher_.setup();
//                func_();
//                exiter_.setup();
//            }))
    {
        thread_.reset(new std::thread([&](){
                        launcher_.setup();
                        func_();
                        exiter_.setup();
                    }));
    }
    ~ThreadGuard()
    {

    }

    void detach()
    {
        thread_->detach();
        launcher_.wait();
    }

    void waitForStop()
    {
        exiter_.waitForStop();
    }

private:
    ThreadGuard(const ThreadGuard &) = delete;
    const ThreadGuard &operator=(const ThreadGuard &) = delete;

private:
    std::function<void()> func_;
    std::unique_ptr<std::thread> thread_;
    Launcher launcher_;
    Exiter exiter_;
};

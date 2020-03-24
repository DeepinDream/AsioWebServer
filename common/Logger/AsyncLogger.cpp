#include "AsyncLogger.h"
#include <assert.h>
// #include <stdio.h>
// #include <unistd.h>
#include "LogFile.h"
#include <functional>

const std::size_t kMAX_FILE_SIZE = 10 * 1024 * 1024;

AsyncLogger::AsyncLogger(std::string logFileName_, int flushInterval)
    : flushInterval_(flushInterval)
    , running_(false)
    , basename_(logFileName_)
    ,
    //      thread_(std::bind(&AsyncLogger::threadFunc, this)),
    currentBuffer_(new Buffer)
    , nextBuffer_(new Buffer)
    , buffers_()
    , fileSlicer_(logFileName_, kMAX_FILE_SIZE)
{
    assert(logFileName_.size() > 1);
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
    thread_.reset(new ThreadGuard(std::bind(&AsyncLogger::threadFunc, this)));
}

AsyncLogger::~AsyncLogger()
{
    if (running_)
        stop();
}

void AsyncLogger::append(const char* logline, int len)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (currentBuffer_->avail() > len)
        currentBuffer_->append(logline, len);
    else {
        buffers_.push_back(currentBuffer_);
        currentBuffer_.reset();
        if (nextBuffer_)
            currentBuffer_ = std::move(nextBuffer_);
        else
            currentBuffer_.reset(new Buffer);
        currentBuffer_->append(logline, len);
        cond_.notify_all();
    }
}

void AsyncLogger::start()
{
    running_ = true;
    //    thread_.detach();
    thread_->detach();
}

void AsyncLogger::stop()
{
    running_ = false;
    cond_.notify_all();
    //    thread_.waitForStop();
    thread_->waitForStop();
}

void AsyncLogger::threadFunc()
{
    assert(running_ == true);
    // LogFile output(basename_);
    LogFile output(fileSlicer_.getLogName());
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while (running_) {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());

        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (buffers_.empty())  // unusual usage!
            {
                cond_.wait_for(lock, std::chrono::seconds(flushInterval_));
            }
            buffers_.push_back(currentBuffer_);
            currentBuffer_.reset();

            currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if (!nextBuffer_) {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());

        if (buffersToWrite.size() > 25) {
            // char buf[256];
            // snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger
            // buffers\n",
            //          Timestamp::now().toFormattedString().c_str(),
            //          buffersToWrite.size()-2);
            // fputs(buf, stderr);
            // output.append(buf, static_cast<int>(strlen(buf)));
            buffersToWrite.erase(buffersToWrite.begin() + 2,
                                 buffersToWrite.end());
        }

        for (size_t i = 0; i < buffersToWrite.size(); ++i) {
            // FIXME: use unbuffered stdio FILE ? or use ::writev ?
            output.append(buffersToWrite[i]->data(),
                          buffersToWrite[i]->length());
        }

        if (buffersToWrite.size() > 2) {
            // drop non-bzero-ed buffers, avoid trashing
            buffersToWrite.resize(2);
        }

        if (!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2) {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}

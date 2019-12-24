#pragma once

#include "LogStream.h"
#include <vector>
#include <memory>
#include <functional>

using AppendFunc = std::function<void(const char *, int)>;
using FlushFunc = std::function<void()>;

struct SwapArea
{
    using Buffer = FixedBuffer<kLargeBuffer>;
    using BufferVector = std::vector<std::shared_ptr<Buffer>>;
    using BufferPtr = std::shared_ptr<Buffer>;

    SwapArea()
        : currentBuffer_(new Buffer),
          nextBuffer_(new Buffer),
          buffers_()
    {
        currentBuffer_->bzero();
        nextBuffer_->bzero();
        buffers_.reserve(16);
    }

    void pushBuffer()
    {
        buffers_.push_back(currentBuffer_);
        currentBuffer_.reset();
        if (nextBuffer_)
            currentBuffer_ = std::move(nextBuffer_);
        else
            currentBuffer_.reset(new Buffer);
    }

    void swap(SwapArea &rhs)
    {
        buffers_.push_back(currentBuffer_);
        currentBuffer_.reset();
        currentBuffer_ = std::move(rhs.currentBuffer_);
        rhs.buffers_.swap(buffers_);
        if (!nextBuffer_)
        {
            nextBuffer_ = std::move(rhs.nextBuffer_);
        }
    }

    void flush(AppendFunc appendFunc, FlushFunc flushFunc)
    {
        assert(!buffers_.empty());

        if (buffers_.size() > 25)
        {
            buffers_.erase(buffers_.begin() + 2, buffers_.end());
        }

        for (size_t i = 0; i < buffers_.size(); ++i)
        {
            // FIXME: use unbuffered stdio FILE ? or use ::writev ?
            appendFunc(buffers_[i]->data(), buffers_[i]->length());
        }

        if (buffers_.size() > 2)
        {
            // drop non-bzero-ed buffers, avoid trashing
            buffers_.resize(2);
        }

        if (!currentBuffer_)
        {
            assert(!buffers_.empty());
            currentBuffer_ = buffers_.back();
            buffers_.pop_back();
            currentBuffer_->reset();
        }

        if (!nextBuffer_)
        {
            assert(!buffers_.empty());
            nextBuffer_ = buffers_.back();
            buffers_.pop_back();
            nextBuffer_->reset();
        }

        buffers_.clear();
        flushFunc();
    }

    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
};

class BufferSwaper
{
public:
    int avail()
    {
        return area_cache_.currentBuffer_->avail();
    }

    void append(const char *logline, int len)
    {
        area_cache_.currentBuffer_->append(logline, len);
    }

    void pushBuffer()
    {
        area_cache_.pushBuffer();
    }

    bool isVectorEmpty()
    {
        return area_cache_.buffers_.empty();
    }

    void swap()
    {
        area_cache_.swap(area_writer_);
    }

    void flush(AppendFunc appendFunc, FlushFunc flushFunc)
    {
        area_writer_.flush(appendFunc, flushFunc);
    }

private:
    SwapArea area_cache_;
    SwapArea area_writer_;
};
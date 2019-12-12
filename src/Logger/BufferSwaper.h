#pragma once

#include "LogStream.h"
#include <vector>
#include <memory>
#include <functional>

struct SwapBuff
{
    using Buffer = FixedBuffer<kLargeBuffer>;
    using BufferVector = std::vector<std::shared_ptr<Buffer>>;
    using BufferPtr = std::shared_ptr<Buffer>;
    SwapBuff()
        : currentBuffer_(new Buffer),
          nextBuffer_(new Buffer),
          buffers_()
    {
        currentBuffer_->bzero();
        nextBuffer_->bzero();
        buffers_.reserve(16);
    }

    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
};

class BufferSwaper
{
public:
    using FlushFunc = std::function<void()>;

    int avail();

    void append(const char *logline, int len);

    void pushBuffer();

    void swap(BufferSwaper &rhs);

    bool isVectorEmpty();

    void flush(FlushFunc func);

private:
    SwapBuff buf_front_;
    SwapBuff buf_back_;
};
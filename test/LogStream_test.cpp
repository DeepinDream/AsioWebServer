#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Logger/LogStream.h"
#include <iostream>

using namespace testing;

TEST(FixedBuffer, test)
{
    {
        FixedBuffer<kSmallBuffer> buffer;
        buffer.append("abc", 3);
        EXPECT_STREQ(buffer.data(), "abc");
    }

    {
        FixedBuffer<2> buffer;
        buffer.append("abcdedf", 7);
        EXPECT_STREQ(buffer.data(), "abc");
    }
}

TEST(LogStream, output)
{
    LogStream log;
    log << 123 << "abc";
    EXPECT_STREQ(log.buffer().data(), "123abc");
}
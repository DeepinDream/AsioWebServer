#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "utils/MyString.h"
#include <iostream>

using namespace testing;

TEST(MyString, StringFormat)
{
    EXPECT_STREQ(utils::StringFormat(std::string("Hello %s"), "world").c_str(), "Hello world");
    EXPECT_STREQ(utils::StringFormat(std::string("Hello %d"), 234).c_str(), "Hello 234");
    EXPECT_STREQ(utils::StringFormat(std::string("Hello %.2f"), 100.499).c_str(), "Hello 100.50");
}
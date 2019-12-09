#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace testing;

int func0()
{
    return 1;
}

TEST(Func, Test0)
{
    EXPECT_EQ(func0(), 1);
}

TEST(Func, Test1)
{
    EXPECT_NE(func0(), 0);
}
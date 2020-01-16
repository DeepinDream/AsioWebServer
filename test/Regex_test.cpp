#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <iostream>
#include <regex>
#include <string>

using namespace testing;

TEST(REGEX, test0)
{
    std::regex e("*/string/?$");
    std::smatch sm_res;
    std::string str("/string/");
    EXPECT_TRUE(std::regex_match(str, sm_res, e));

    std::string str2("/string/abc");
    EXPECT_FALSE(std::regex_match(str2, sm_res, e));
    std::cout << sm_res.str() << std::endl;
}

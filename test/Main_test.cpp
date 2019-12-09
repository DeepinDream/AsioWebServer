#include <iostream>
#include "gmock/gmock.h"
#include "gtest/gtest.h"


using namespace std;
int main(int argc,char* argv[])
{ 
    std::cout << "Hello Test!" << std::endl;
    //testing::GTEST_FLAG(output) = "xml:"; //生成xml结果文件
    testing::InitGoogleTest(&argc,argv); //初始化
    if( RUN_ALL_TESTS())  //跑单元测试
	        return 0;
}
// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "../Logger.h"
#include <string>
#include <unistd.h>
#include <vector>
#include <memory>
#include <iostream>
#include <chrono>
#include "../ThreadGuard.h"
using namespace std;

void threadFunc()
{
    for (int i = 0; i < 100000; ++i)
    {
        LOG_INFO << i;
    }
}

void type_test()
{
    // 13 lines
    cout << "----------type test-----------" << endl;
    LOG_INFO << 0;
    LOG_INFO << 1234567890123;
    LOG_INFO << 1.0f;
    LOG_INFO << 3.1415926;
    LOG_INFO << (short) 1;
    LOG_INFO << (long long) 1;
    LOG_INFO << (unsigned int) 1;
    LOG_INFO << (unsigned long) 1;
    LOG_INFO << (long double) 1.6555556;
    LOG_INFO << (unsigned long long) 1;
    LOG_INFO << 'c';
    LOG_INFO << "abcdefg";
    LOG_INFO << string("This is a string");
}

void stressing_single_thread()
{
    // 100000 lines
    cout << "----------stressing test single thread-----------" << endl;
    for (int i = 0; i < 100000; ++i)
    {
        LOG_INFO << i;
    }
}

void stressing_multi_threads(int threadNum = 4)
{
    // threadNum * 100000 lines
    cout << "----------stressing test multi thread-----------" << endl;
    std::vector<ThreadGuard*> vsp;
    for(int i = 0; i < threadNum; i++){

        vsp.push_back(new ThreadGuard(threadFunc));
    }
    for (int i = 0; i < threadNum; ++i)
    {
        vsp[i]->detach();
    }
    for (int i = 0; i < threadNum; ++i)
    {
        vsp[i]->waitForStop();
    }
    // sleep(3);
}

void other()
{
    // 1 line
    cout << "----------other test-----------" << endl;
    LOG_INFO << "fddsa" << 'c' << 0 << 3.666 << string("This is a string");
}

class Tester
{
public:
    void test()
    {
            cout << "----------Tester test-----------" << endl;
    LOG_INFO << 0;
    LOG_WANRING << 1234567890123;
    LOG_ERROR << 1.0f;
    LOG_INFO << 3.1415926;
    LOG_WANRING << (short) 1;
    LOG_INFO << (long long) 1;
    LOG_ERROR << (unsigned int) 1;
    LOG_INFO << (unsigned long) 1;
    LOG_INFO << (long double) 1.6555556;
    LOG_WANRING << (unsigned long long) 1;
    LOG_INFO << 'c';
    LOG_ERROR << "abcdefg";
    LOG_INFO << string("This is a string");
    }
};


int main()
{
    auto begin = std::chrono::system_clock::now();
    Logger::setLogLevel(LogLevel::INFO);
    // 共500014行
    type_test();
    Tester().test();
    sleep(3);

    stressing_single_thread();
    sleep(3);

    other();
    sleep(3);

    stressing_multi_threads();
    sleep(3);

    auto end = std::chrono::system_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    std::cout << "costed time: " << duration.count() << " milliseconds" << std::endl;
    return 0;
}
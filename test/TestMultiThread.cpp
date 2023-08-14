#include <iostream>
#include <cstdint>
#include <thread>
#include <vector>
#include <sstream>

#include "Atomic128.hpp"

struct alignas(16) TestData
{
    uint32_t first;
    uint32_t second;
    uint64_t third;

    friend std::ostream& operator<<(std::ostream& os, const TestData& data)
    {
        os << "first: " << data.first << ", second: " << data.second << ", third: " << data.third;
        return os;
    }
};

A128::Atomic128<TestData> data(0, 0, 0);

void ThreadTest(uint32_t tid)
{
    TestData current = {0, 0, 0};
    TestData desired = {1, 1, 1};
    bool success;

    for (int i = 0; i < 100000; ++i)
    {
        success = false;
        while (!success)
        {
            desired.first = current.first + 1;
            desired.second = current.second + 1;
            desired.third = current.third + 1;

            success = data.CompareExchange(current, desired);
            if (success)
            {
//                std::stringstream ss;
//                ss << "Thread " << tid << " succeeded: " << desired << std::endl;
//                std::cout << ss.str();
                current = desired;
            }
            else
            {
                std::stringstream ss;
                ss << "Thread " << tid << " failed, retrying" << std::endl;
                std::cout << ss.str();
            }
        }
    }
}

int main()
{
    std::vector<std::thread> threads;

    for (int i = 0; i < 8; ++i)
    {
        threads.emplace_back(ThreadTest, i);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    std::cout << "Final value: " << data.Load() << std::endl;
}
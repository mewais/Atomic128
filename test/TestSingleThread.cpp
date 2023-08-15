#include <iostream>
#include <cstdint>

#include "Atomic128.hpp"

struct alignas(16) TestData
{
    uint32_t first;
    uint32_t second;
    uint64_t third;

    TestData() : first(0), second(0), third(0)
    {
    }

    TestData(uint32_t first, uint32_t second, uint64_t third) : first(first), second(second), third(third)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const TestData& data)
    {
        os << "first: " << data.first << ", second: " << data.second << ", third: " << data.third;
        return os;
    }
};

int main()
{
    A128::Atomic128<TestData> data(0, 0, 0);

    // Initialize the data
    TestData init_data = {42, 99, 123456789ULL};
    TestData new_data = {101, 202, 987654321ULL};
    TestData result_data;

    // Test Load
    result_data = data.Load();
    std::cout << "Initial data: " << result_data << std::endl;

    // Test Store and Load
    data.Store(init_data);
    result_data = data.Load();
    std::cout << "Stored data: " << result_data << std::endl;

    // Test Exchange
    result_data = data.Exchange(new_data);
    std::cout << "Exchanged data: " << result_data << ", Now holds: " << data.Load() << std::endl;

    // Test CompareExchange
    bool success = data.CompareExchange(new_data, init_data);
    if (success)
    {
        std::cout << "CompareExchange success, now holds " << data.Load() << std::endl;
    }
    else
    {
        std::cout << "CompareExchange failed, expected: " << init_data << ", found: " << data.Load() << std::endl;
    }
}
#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <string_view>

using namespace std;
using namespace Catch::Matchers;

TEST_CASE("Test")
{
    const int number = 67;

    constexpr double pi = 3.14;  
    constexpr double two_pi = pi * 2.0;

    constexpr size_t size = 1024;

    int tab[size] = {};
    std::array<int, size> arr = {};
}

namespace Cpp11
{
    constexpr int factorial(int n)
    {
        return (n == 0) ? 1 : n * factorial(n - 1);
    }
}

constexpr int factorial(int n)
{
    if (n == 0)
        return 1;
    else
        return n * factorial(n - 1);
}

TEST_CASE("factorial")
{
    static_assert(factorial(3) == 6);

    int tab[factorial(4)] = {};

    static_assert(std::size(tab) == 24);
}

template <size_t N>
constexpr std::array<uint64_t, N> create_factorial_lookup()
{    
    constexpr size_t head_size = 5;
    constexpr std::array head_lookup = { 0, 1, 2, 6, 24 };

    std::array<uint64_t, N> result{};

    constexpr auto min_size = std::min(head_size, N);

    auto head_lookup_end = head_lookup.begin() + min_size;
    std::copy(head_lookup.begin(), head_lookup_end, result.begin());

    for (size_t i = min_size; i < N; ++i)
        result[i] = factorial(i);

    return result;
}

void print()
{
    constexpr std::array local_lookup = create_factorial_lookup<10>();

    for (const auto& item : local_lookup)
        std::cout << item << " ";
    std::cout << "\n";
}

TEST_CASE("run-time usage")
{
    auto facs = create_factorial_lookup<10>();

    facs[4] *= 2;

    REQUIRE(facs[4] == 48);
}

TEST_CASE("factorial lookup")
{
    constexpr std::array factorial_lookup = create_factorial_lookup<10>();
    //constexpr std::array factorial_lookup = { 0, 1, 2, 6, 24 };

    static_assert(factorial_lookup[4] == 24);

    std::array runtime_lookup = create_factorial_lookup<6u>();

    auto constexpr_lambda = [](int a) { return factorial(2 * a); };

    static_assert(constexpr_lambda(2) == 24);

    if constexpr(constexpr auto pos = 
            std::find_if(factorial_lookup.begin(), factorial_lookup.end(), [](auto item) { return item > 1000; }); pos != factorial_lookup.end())
    {
        std::cout << "Found: " << *pos << "\n";
    }
    else
    {
        std::cout << "Value not found!\n";
    }

    print();
}

struct Data
{
    int id;
    std::array<int, 255> data;

    void print() const
    {}
};

TEST_CASE("aggregate initialization")
{
    Data d1{ 1, {1, 2, 3} };
}

TEST_CASE("C++17")
{
    std::vector vec = { 1, 2, 3 };

    SECTION("if with initializer")
    {
        if (auto pos = std::find(std::begin(vec), std::end(vec), 2); pos != std::end(vec))
        {
            std::cout << "Found: " << *pos << "\n";
        }
        else
        {
            assert(pos == std::end(vec));
        }
    }
}
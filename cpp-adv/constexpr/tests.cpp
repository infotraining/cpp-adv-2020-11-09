#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <array>

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
constexpr std::array<int, N> create_factorial_lookup()
{
    std::array<int, N> result{};

    for (size_t i = 0; i < N; ++i)
        result[i] = factorial(i);

    return result;
}

TEST_CASE("factorial lookup")
{
    constexpr std::array factorial_lookup = create_factorial_lookup<5u>();

    static_assert(factorial_lookup[4] == 24);

    std::array runtime_lookup = create_factorial_lookup<6u>();

    auto constexpr_lambda = [](int a) { return factorial(2 * a); };

    static_assert(constexpr_lambda(2) == 24);
}
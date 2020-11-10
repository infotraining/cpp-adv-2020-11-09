#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <optional>

using namespace std;

template <typename T>
T maximum(T const& a, T const& b)
{
    //undeclared(); // ERROR - phase 1
    // puts(__FUNCSIG__);
    return a < b ? b : a; // maybe ERROR - phase 2
}

const char* maximum(const char* a, const char* b)
{
    return strcmp(a, b) > 0 ? a : b;
}


TEST_CASE("Test")
{
    auto ptr_fun = &maximum<int>;

    REQUIRE(maximum(1, 2) == 2);
    REQUIRE(maximum(3.14, 2.71) == Approx(3.14));

    std::string text = "def";
    REQUIRE(maximum<std::string>("abc", text) == text);

    std::vector vec = { 1, 2, 3 };
    REQUIRE(maximum(vec, vector{ 4, 5, 6 }) == vector{ 4, 5, 6 });

    const char* ctxt1 = "abc";
    const char* ctxt2 = "def";

    REQUIRE(maximum(ctxt1, ctxt2) == "def"s);
}

template <typename TResult, typename T1, typename T2>
TResult multiply1(T1 a, T2 b)
{
    return a * b;
}

template <typename T1, typename T2>
auto multiply2(T1 a, T2 b)
{
    return a * b;
}

template <typename T1, typename T2>
std::common_type_t<T1, T2> multiply3(T1 a, T2 b)
{
    using CType = common_type_t<T1, T2>;
    CType value = a * b;
    
    return value;
}

TEST_CASE("auto")
{
    int x = 10;
    const int cx = 20;
    int& ref_x = x;
    const int& cref_x = cx;

    auto ax1 = cx; // int
    auto ax2 = ref_x; // int
    auto ax3 = cref_x; // int
}

TEST_CASE("common type")
{
    using CT1 = common_type_t<int, char>;
    static_assert(is_same_v<CT1, int>);
}

template <typename Container>
decltype(auto) element_at(Container& container, size_t index)
{
    auto it = begin(container);

    advance(it, index);

    return *it; // *it -> int&
}

TEST_CASE("templates & return types")
{
    auto result1 = multiply1<int64_t>(32, 256);

    auto result2 = multiply2(2, 3.14);
    REQUIRE(result2 == Approx(6.28));

    SECTION("auto vs decltype(auto)")
    {
        list lst = { 1, 2, 3, 4 };
        REQUIRE(element_at(lst, 2) == 3);
        element_at(lst, 0) = 42;

        REQUIRE(lst.front() == 42);
    }
}

template <typename Iter, typename Predicate>
constexpr Iter my_find_if(Iter first, Iter last, Predicate predicate)
{
    // puts(__FUNCSIG__);
    for (auto it = first; it != last; ++it)
        if (predicate(*it))
            return it;
    return last;
}

//constexpr auto my_find_if(auto first, auto last, auto predicate)
//{
//    // puts(__FUNCSIG__);
//    for (auto it = first; it != last; ++it)
//        if (predicate(*it))
//            return it;
//    return last;
//}

bool is_even(int64_t x) { return x % 2 == 0; }

TEST_CASE("generic algorithm")
{
    vector<int32_t> vec = { 1, 5, 7, 9, 42, 33, 66 };

    auto pos = my_find_if(vec.begin(), vec.end(), is_even);
    REQUIRE(*pos == 42);

    pos = my_find_if(vec.begin(), vec.end(), [](int x) { return x % 2 == 0; });
    REQUIRE(*pos == 42);
}

template <typename T>
class Holder
{
    T item_;
public:
    using value_type = T;

    Holder(const T& value)
        : item_{value}
    {}

    Holder(T&& value) : item_{std::move(value)}
    {}

    template <typename... TValue>
    Holder(std::in_place_t, TValue&&... value) : item_(std::forward<TValue>(value)...)
    {
        puts(__FUNCSIG__);
    }

    Holder(const Holder&) = default;

    const T& value() const
    {
        return item_;
    }

    T& value()
    {
        return item_;
    }

    void info() const
    {
        std::cout << "Holder<T: " << typeid(T).name() << ">(" << item_ << ")\n";
    }
};

template <typename T>
class Holder<T*>
{
    T* ptr_;
public:
    using value_type = T;

    Holder(T* ptr) : ptr_{ptr}
    {}

    Holder(const Holder&) = delete;
    Holder& operator=(const Holder&) = delete;

    Holder(Holder&& other) noexcept
        : ptr_{ other.ptr_ }
    {
        other.ptr_ = nullptr;
    }

    Holder& operator=(Holder&& other) noexcept
    {
        if (this != &other)
        {
            this->~Holder();

            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }

        return *this;
    }

    ~Holder() noexcept
    {
        delete ptr_;
    }

    const T& value() const
    {
        assert(ptr_ != nullptr);
        return *ptr_;
    }

    T& value()
    {
        assert(ptr_ != nullptr);
        return *ptr_;
    }

    void info() const
    {
        std::cout << "Holder<T*: " << typeid(T).name() << ">(" << *ptr_ << " - " << ptr_ << ")\n";
    }
};

template <>
class Holder<const char*>
{
    const char* text_;

public:
    using value_type = const char*;

    Holder(const char* text)
        : text_{ text }
    {
    }

    string_view value() const
    {
        return text_;
    }

    void info() const
    {
        cout << "Holder<const char*>(" << text_ << ")" << endl;
    }
};

TEST_CASE("Holder")
{
    Holder<int> hint = 4;
    REQUIRE(hint.value() == 4);

    Holder<int> backup = hint;

    Holder<std::string> text(std::in_place, 10u, 'a');
    text.info();


    Holder<int*> hptr = new int(13);
    REQUIRE(hptr.value() == 13);   
    hptr.value() = 42;
    hptr.info();

    Holder<const char*> ctext = "text";

    std::cout << ctext.value() << "\n";

} // hptr releases memory

TEST_CASE("optional & std::in_place")
{
    std::optional<int> opt_int = 4;
    REQUIRE(opt_int.has_value());
    REQUIRE(*opt_int == 4);

    std::optional<std::string> middle_name(std::in_place, "Janek", 3);
    std::cout << *middle_name << "\n";
}

template <typename T>
struct Item
{
    Holder<T> item;

    Item(T item) : item(item)
    {
    }

    //logic
};

TEST_CASE("Data")
{
    Item d1 = 4;

    REQUIRE(d1.item.value() == 4);

    Item d2 = new double(3.14);

    REQUIRE(d2.item.value() == Approx(3.14));

    Item d3 = "text";

    REQUIRE(d3.item.value().size() == 4);
}
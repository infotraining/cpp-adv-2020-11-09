#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <tuple>

#include "catch.hpp"

using namespace std;

string full_name(const string& first, const string& last)
{
    return first + " " + last;
}

TEST_CASE("reference binding")
{
    std::string name = "jan";

    SECTION("C++98")
    {
        std::string& ref_name = name;

        const std::string& ref_full_name = full_name(name, "Kowalski");
        REQUIRE(ref_full_name.size() == 12);
        //ref_full_name[0] = 'J';
    }

    SECTION("C++11")
    {
        std::string&& rv_ref_full_name = full_name(name, "Kowalski");
        REQUIRE(rv_ref_full_name.size() == 12);
        rv_ref_full_name[0] = 'J';

        REQUIRE(rv_ref_full_name == "Jan Kowalski");

        //std::string&& rv_ref_name = name; // ERROR - ill formed
    }
}

std::vector<std::string> create_and_fill_rvo()
{
    return std::vector{"one"s, "two"s, "three"s}; // prvalue
}

std::vector<std::string> create_and_fill_nrvo()
{
    std::vector<std::string> vec;

    std::string str = "text";

    vec.push_back(str);

    vec.push_back(str + str);

    vec.push_back("text");

    vec.push_back(std::move(str));

    return vec; // lvalue
}

TEST_CASE("create & fill")
{
    std::vector vec1 = create_and_fill_rvo(); 
    std::vector vec2 = create_and_fill_nrvo(); // NRVO (Named Return Value Optimization)
}

class Array
{
private:
	size_t size_;
	int* data_;
public:
	typedef int* iterator; // legacy style
	using const_iterator = const int*; // since C++11

									   // allows list initialization: Array a = {1, 2, 3}
	Array(std::initializer_list<int> il)
		: size_(il.size()), data_(new int[il.size()])
	{
		std::copy(il.begin(), il.end(), data_);
		std::cout << "Array({ ";
		for (const auto& item : il)
			std::cout << item << " ";
		std::cout << "})\n";
	}

	// copy constructor
	Array(const Array& source) : size_(source.size_), data_(new int[source.size_])
	{
		std::copy(source.begin(), source.end(), this->data_);
		std::cout << "Array(const Array& - copy constructor)\n";
	}

	// copy assignment operator
	Array& operator=(const Array& source)
	{
		if (this != &source) // protection from self-assignment
		{
			delete[] data_; // free memory

			// copy of state from source object
			size_ = source.size_;
			data_ = new int[size_];
			std::copy(source.begin(), source.end(), data_);
		}

		std::cout << "Array operator=(const Array& - copy assignment)\n";
		return *this;
	}

	// destructor
	~Array()
	{
		std::cout << "~Array()\n";
		delete[] data_;
	}

	iterator begin()
	{
		return data_;
	}

	const_iterator begin() const
	{
		return data_;
	}

	iterator end()
	{
		return data_ + size_;
	}

	const_iterator end() const
	{
		return data_ + size_;
	}

	void reset(int value)
	{
		std::fill_n(data_, size_, value);
	}

	size_t size() const
	{
		return this->size_;
	}

	int& operator[](size_t index)
	{
		return data_[index];
	}

	const int& operator[](size_t index) const
	{
		return data_[index];
	}
};

bool operator==(const Array& lhs, const Array& rhs)
{	
	return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

TEST_CASE("Array")
{
	Array arr = { 1, 2, 3 };

	Array backup = arr;

	REQUIRE(backup == arr);
}
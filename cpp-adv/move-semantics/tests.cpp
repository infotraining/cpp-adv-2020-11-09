#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <tuple>
#include <memory>

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

	Array(Array&& source) noexcept : size_{source.size_}, data_{source.data_} // transfer of state
	{	
		// set to resourceless state
		source.size_ = 0; // optional
		source.data_ = nullptr; // mandatory

		std::cout << "Array(Array&& - move constructor)\n";
	}

	Array& operator=(Array&& source) noexcept
	{
		if (this != &source) // a = std::move(a) - self assignment protection
		{
			delete[] data_;

			size_ = source.size_;
			data_ = source.data_;

			source.size_ = 0; // optional
			source.data_ = nullptr; // mandatory
		}
		std::cout << "Array operator=(Array&& - move assignment)\n";

		return *this;
	}

	// destructor
	~Array() noexcept
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

Array create_array()
{
	Array arr = { 1, 2, 3, 4, 5 };

	for (auto& item : arr)
		item *= 2;

	return arr;
}

TEST_CASE("Array")
{
	Array arr = create_array();

	Array other = std::move(arr); // move constructor

	Array another = { 1, 2, 3 };

	another = std::move(other); // move assignment

	REQUIRE(another == Array{ 2, 4, 6, 8, 10 });

	SECTION("move does not move")
	{
		const Array carr = { 1, 2, 3 };

		Array target = std::move(carr);

		REQUIRE(carr.size() == 3);
	}
}

struct Data
{
	int id;
	std::string name;
	Array data;

	//Data(int id, std::string n, Array d)
	//	: id{ id }, name(std::move(n)), data(std::move(d))
	//{
	//}

	template <typename TName, typename TArray>
	Data(int id, TName&& name, TArray&& data)
		: id{ id }, name(std::forward<TName>(name)), data(std::forward<TArray>(data))
	{
	}

	Data(const Data&) = default;
	Data& operator=(const Data&) = default;
	Data(Data&&) = default;
	Data& operator=(Data&&) = default;
	~Data() = default;

	void print() const
	{
		std::cout << "Data(" << id << ", \"" << name << "\", [ ";
		for (const auto& item : data)
			std::cout << item << " ";
		std::cout << "])\n";
	}
};

TEST_CASE("Data - copy & move semantics")
{
	std::cout << "\n--------------------\n";

	Data d1{ 1, "d1", Array{1, 2, 3} };
	d1.print();

	Data d2 = d1;
	d2.print();

	Data d3 = std::move(d1);
	d3.print();

	d1.print();
}

TEST_CASE("std::vector - move semantics")
{
	std::cout << "\n--------------------\n";

	Array arr{ 1, 2, 3 };

	std::vector<Array> vec;

	vec.push_back(std::move(arr));
	std::cout << "\n--------------------\n";
	vec.push_back(Array{ 4, 5, 6 });
	std::cout << "\n--------------------\n";
	vec.emplace_back(std::initializer_list<int>{ 7, 8, 9 });
	std::cout << "\n--------------------\n";
	vec.emplace_back(std::initializer_list<int>{ 2, 1, 0 });
	std::cout << "\n--------------------\n";
	vec.push_back({ 56 });
	std::cout << "\n--------------------\n";

	Array target = std::move_if_noexcept(vec[0]);
}

struct Gadget
{
	std::string name;
};

void have_fun(Gadget& g)
{
	puts(__FUNCSIG__);
	std::cout << "Having fun with " << g.name << "\n";
}

void have_fun(const Gadget& g)
{
	puts(__FUNCSIG__);
	std::cout << "Having fun with " << g.name << "\n";
}

void have_fun(Gadget&& g)
{
	puts(__FUNCSIG__);
	std::cout << "Having fun with " << g.name << "\n";
}

//void use(Gadget& g)
//{
//	puts(__FUNCSIG__);
//	have_fun(g);
//}
//
//void use(const Gadget& g)
//{
//	puts(__FUNCSIG__);
//	have_fun(g);
//}
//
//
//void use(Gadget&& g)
//{
//	puts(__FUNCSIG__);
//	have_fun(std::move(g));
//}



template <typename T>
void use(T&& g)
{
	puts(__FUNCSIG__);
	have_fun(std::forward<T>(g));
}

TEST_CASE("perfect forwarding")
{
	Gadget g{ "g" };
	const Gadget cg{ "cg" };

	use(g);
	use(cg);
	use(Gadget{ "temp gadget" });
}

TEST_CASE("auto")
{
	auto&& aref1 = 42; // int&&

	int x = 665;
	auto&& aref2 = x;  // int&
}

TEST_CASE("perfect forwarding - use cases")
{
	std::vector<Data> vec;

	vec.emplace_back(1, "item1", Array{ 1, 2, 3 });
	vec.emplace_back(2, "item2", Array{ 4, 5, 6 });

	std::string text = "item3";
	std::unique_ptr<Data> ptr1 = std::make_unique<Data>(3, text, Array{ 7, 7, 8 });
	ptr1->print();	
}


using Row = Array;

struct DataSet
{
	std::vector<Row> rows;

	template <typename TRow>
	void add(TRow&& r)
	{
		rows.emplace_back(std::forward<TRow>(r));
	}
};

TEST_CASE("DataSet")
{
	std::cout << "\n==============================\n";

	DataSet ds;

	ds.add(Array{ 1, 2, 3 });
}
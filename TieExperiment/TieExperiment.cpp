#include <assert.h>
#include <set>
#include <iostream>
#include <tuple>
#include <ostream>
#include <string>

// alignas
struct alignas(alignof(float)) type_f
{
	float data_;
};

struct alignas(8) type_i
{
	int data_;
	int data2_;
	int data3_;
};

struct alignas(64) empty_t
{};

class NoCopyable
{
protected:
	NoCopyable() = default;

public:
	NoCopyable(const NoCopyable&) = delete;
	NoCopyable& operator=(const NoCopyable&) = delete;
};

class NoMoveable
{
private:
	NoMoveable(NoMoveable&&) = delete;
	NoMoveable& operator=(NoMoveable&&) = delete;
};

// prevent implicit conversion 
void integral_only(int n) {};
void integral_only(float n) = delete;

//	final - keyword

struct A {
	virtual void foo() const = 0;
};

struct B : A{
	void foo() const final override {};
	virtual void bar() const {};
};

struct C final : B
{
	// void foo() const override {};
	void bar() const override {};
};

struct Record
{
	std::string name;
	int age;
	float weight;

	bool operator< (const Record& rhs) const
	{
		return std::tie(name, age, weight) < std::tie(rhs.name, rhs.age, rhs.weight);
	}

	friend std::ostream& operator<< (std::ostream& os, const Record& r)
	{
		os << "[" << r.name << "," << r.age << "," << r.weight << "]" << std::endl;
		return os;
	}
};

template<typename function_type, typename... arg_types>
decltype(auto) do_nothing_but_forward(function_type func, arg_types&&... args) {
	return func(std::forward<arg_types>(args)...);
}

int main()
{
	std::set<Record> set;
	Record r{ "Max", 18, 70.f };
	std::set<Record>::iterator it;
	bool inserted{ false };
	std::tie(it, inserted) = set.insert(r);
	set.insert({ "Robert", 23, 84.f });
	set.insert({ "Anna", 31, 61.f });

	if (inserted)
		std::cout << "Value has been inserted successfully." << std::endl;
	std::cout << *it;

	auto test = []() { return false; };
	inserted = test();

	return 0;
}
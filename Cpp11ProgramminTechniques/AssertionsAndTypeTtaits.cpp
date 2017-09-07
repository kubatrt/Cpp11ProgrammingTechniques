#include <iostream>
#include <type_traits>
#include <unordered_map>
#include <functional>
#include <typeindex>

template<class T>
T bitwise_and(T a, T b) {
	static_assert(std::is_integral<T>::value, "an error");
	return a % b;
}


void staticAssert()
{
	// 1. Basic static assert example
	int x = 1234123, y = 1;
	std::cout << bitwise_and(x, y) << std::endl;

	unsigned char c = 1, d = 2;
	std::cout << bitwise_and(c, d) << std::endl;

}

void UsingTypeInfoAtRuntime()
{
	int i{ 0 };
	double d{ 1.1 };
	float f { 3.14f };
	std::string s{ "string" };

	std::cout << "i = " << typeid(i).name() << std::endl;
	std::cout << "d = " << typeid(d).name() << std::endl;
	std::cout << "f = " << typeid(f).name() << std::endl;
	std::cout << "s = " << typeid(s).name() << std::endl;

	std::unordered_map< std::type_index, std::is_function < std::string() >> type_func;

	
}

struct A {};
struct B : A {};

typedef B C;

void TypeRelationships()
{
	std::cout << std::boolalpha << std::is_base_of<A, B>::value << std::endl;
	std::cout << std::boolalpha << std::is_base_of<B, C>::value << std::endl;

	std::cout << std::boolalpha << std::is_convertible<A, B>::value << std::endl;
	std::cout << std::boolalpha << std::is_convertible<B, A>::value << std::endl;

	std::cout << std::boolalpha << std::is_same<A, B>::value << std::endl;
	std::cout << std::boolalpha << std::is_same<B, C>::value << std::endl;
	//std::is_
}

void PropertyQueries()
{
	/*
		aligment_of: number of bytes in memory of the object
		extent: number of elements in multi-dimensional array
		rank: number of dimensions
	*/
}

int func()
{
	return func();
}

int main(int argc, char**argv)
{
	return func();
}
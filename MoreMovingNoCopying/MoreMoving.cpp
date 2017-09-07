#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <sstream>
#include <string>
#include <utility>

using move_only = std::unique_ptr<int>;

template<class T>
struct rref_wrapper
{
	explicit rref_wrapper(T&& v) : val_(move(v)) {}
	explicit operator T() const { return T{ move(val_) }; }

private:
	T&& val_;
};

// only usable on temporaries
template<typename T>
typename std::enable_if<!std::is_lvalue_reference<T>::value, rref_wrapper<T> >::type 
rref(T&& v) 
{
	return rref_wrapper<T>(move(v));
}

// lvalue reference can go away
template<typename T>
void rref(T&) = delete;

// won't work
// std::vector<move_only> v2{ rref(move_only()), rref(move_only()), rref(move_only()) };


// fails
/* vector<unique_ptr<string>> vs {
unique_ptr<string>{ new string{"Doug"} },
unique_ptr<string>{ new string{"Adams"} }
};

vector<unique_ptr<string>> vs {
move(make_unique<string>("Doug")),
move(make_unique<string>("Adams"))
};*/


//-----------------------------------------------------------------------------

template<typename T>
void rev(const T& collection) {
	/*typename*/ T::const_iterator it;
	for (it = collection.begin(); it != collection.end(); ++it)
	{
		// something
	}
}

template <typename T1, typename T2>
void inner(T1& one, T2& two)
{

}

template <typename T1, typename T2>
void outer(T1&& one, T2&& two)
{
	//inner(forward<T1>(one), forward<T2>(two));
}

// just Ok
move_only createMO(int value) {
	move_only b = move_only(new int(value));
	return move(b);
}

void TEST_moving()
{
	outer(1, 2);

	move_only mo = createMO(1);
	// unique_ptr<int> ptr(7); // error
	move_only ptr2 = std::make_unique<int>(10);
	move_only ptr3 = move_only(new int(9));
	// unique_ptr<int> ppJ(ptr);	// error

	std::vector<move_only> vec;
	vec.push_back(std::move(ptr2));
	vec.push_back(std::move(ptr3));
	vec.emplace_back(new int(43));

	std::initializer_list<rref_wrapper<move_only>> il{
		rref(move_only()),
		rref(move_only()),
		rref(move_only())
	};

	std::vector<move_only> vm(il.begin(), il.end());
	rev(vm);
}

// http://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c/
// ----------------------------- perfect forwarding -------------------------------

class A {
protected:
	int a;
public:
	A() : a(0) {}
};

class B : public A {
protected:
	int b;
public:
	B() : A(), b(0) {};
};

class object_counter
{
public:
	static unsigned int count;
protected:
	unsigned int oc_;
};

unsigned int object_counter::count = 0;


class MyKlass : public object_counter
{
public:
	MyKlass(int n, float f)
		: n_(n)
		, f_(f)
		// https://stackoverflow.com/questions/7405740/how-can-i-initialize-base-class-member-variables-in-derived-class-constructor#7405776
		//, oc_(0) // ?
	{
		oc_ = object_counter::count++;
		std::cout << "MyKlass construct" << oc_ << std::endl;
	}
	MyKlass(const MyKlass& obj) 
	{
		n_ = obj.n_;
		f_ = obj.f_;
		std::cout << "MyKlass copy construct " << oc_ << std::endl;
	}
	~MyKlass() 
	{
		std::cout << "MyKlass destruct " << oc_ << std::endl;
	}

	friend std::ostream& operator<<(std::ostream& os, const MyKlass& my)
	{
		std::ostringstream ss;
		ss << "MyKlass: " << my.n_ << ", " << my.f_ << std::endl;
		os << ss.str();
		return os;
	}

private:
	int n_;
	float f_;
};

void addToVector()
{
	std::vector<MyKlass> v;

	//v.push_back(MyKlass(2, 3.14f));
	v.emplace_back(2, 3.14f);
}

// reference collapsing rule
template <typename T>
void f(T&& t)	// special meaning in type deduce concept
{
	T& k = t;
}

template <typename T1, typename T2>
void func(T1 t1, T2 t2)
{
	std::cout << t1 << ", " << t2 << std::endl;
}



template <typename T1, typename T2>
void wrapper(T1&& e1, T2&& e2)
{
	func(std::forward<T1>(e1), std::forward<T2>(e2));
}

template<typename T>
T&& forwarding(typename std::remove_reference_t<T> t) noexcept
{
	return static_cast<T&&>(t);
}

// std::forward
//template<class T>
//T&& forward(typename std::remove_reference<T>::type& t) noexcept {
//	return static_cast<T&&>(t);
//}

// Scott Myers gave the name "universal references" to rvalues that appear in a 
// type-deducing context.


// Higher order functions are functions that may take other functions as arguments or return them.

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
	return unique_ptr<T>(new T(std::forward<Args>(args)...));
}


void TEST_PerfectForwarding()
{
	// pf by emplace()... family methods
	addToVector();
	int i = 44;
	int j = 12;
	f<int&>(i);
	wrapper(i, j);
	MyKlass my(1, 5.4f);
	std::cout << my << std::endl;
}

int main()
{
	TEST_moving();
	TEST_PerfectForwarding();
	
	return 0;
}
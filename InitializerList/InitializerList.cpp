#include <iostream>
#include <vector>
#include <memory>
#include <utility>

// unique_ptr
// http://www.stroustrup.com/C++11FAQ.html#std-unique_ptr
// https://stackoverflow.com/questions/27585014/initialize-an-array-of-unique-ptrs#27585235
// initializer_list
// https://stackoverflow.com/questions/8468774/can-i-list-initialize-a-vector-of-move-only-type?noredirect=1&lq=1


using namespace std;

using Type = int;
using TypeP = unique_ptr<int>;
using Vec = vector<int>;
using VecP = vector<unique_ptr<int>>;
using move_only = std::unique_ptr<int>;

/*
The synopsis of <initializer_list> in 18.9 makes it reasonably clear that elements of an initializer list
are always passed via const-reference. Unfortunately, there does not appear to be any way of using
move-semantic in initializer list elements in the current revision of the language.
*/

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
typename enable_if<!is_lvalue_reference<T>::value, rref_wrapper<T> >::type
rref(T&& v)
{
	return rref_wrapper<T>(move(v));
}

// lvalue reference can go away
template<typename T>
void rref(T&) = delete;

// won't work
// std::vector<move_only> v2{ rref(move_only()), rref(move_only()), rref(move_only()) };

// TODO: make it template
struct Container
{
	Container() = default;
	//Contaner(const vector<unique_ptr>& v) : vec_(v) {}
	Container(std::initializer_list<rref_wrapper<move_only>> list)
		: vec_(list.begin(), list.end())
	{}
	Container(const Container&) = delete;
	Container(Container&&) = delete;
	~Container() = default;

	void add(move_only& p)
	{
		vec_.emplace_back(move(p));
	}

	void print() const
	{
		for (const auto& v : vec_)
		{
			cout << *v << ", ";
		}
		cout << endl;
	}
private:
	std::vector<move_only> vec_;
};

// fails
/* vector<unique_ptr<string>> vs {
unique_ptr<string>{ new string{"Doug"} },
unique_ptr<string>{ new string{"Adams"} }
};

vector<unique_ptr<string>> vs {
move(make_unique<string>("Doug")),
move(make_unique<string>("Adams"))
};*/



void TEST_container()
{
	Container container{
		rref(make_unique<int>(1)),
		rref(make_unique<int>(2)),
		rref(make_unique<int>(3))
	};

	move_only ptr{ new int(4) };
	container.add(ptr);
	container.add(move_only{ new int(5) });

	container.print();
}

int main()
{
	TEST_container();
}
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <iterator>
#include <utility>

// Move semantics
// https://www.youtube.com/watch?v=dcgqUb1SXPk&index=2&list=PLYv8aT6pTPX6mWNRKAEzF4rngHRxb9qlJ
// https://stackoverflow.com/questions/4523178/how-to-print-out-all-elements-in-a-stdstack-or-stdqueue-conveniently
// https://stackoverflow.com/questions/32092434/using-ostream-iterator-and-operator-to-display-a-vector-of-pointers
// std::transform

struct Item
{
	std::string name;
	unsigned int id;

	// need for perfect forwarding
	Item(std::string n, unsigned int i) : name(n), id(i) {}


	Item() = default;
	// the rule of 5
	Item(const Item& item) = default;
	Item(Item&& item) = default;
	Item& operator=(const Item& item) = default;
	Item& operator=(Item&& item) = default;
	~Item() = default;

	friend inline bool operator==(const Item& l, const Item& r) { 
		return l.name == r.name; 
	};
	friend inline bool operator<(const Item& l, const Item& r) { 
		return l.name < r.name; 
	};
	friend inline std::ostream& operator<<(std::ostream& os, const Item& item) {
		os << "[" << item.name << "," << item.id << "]";
		return os;
	}
};

class Backpack
{
public:
	Backpack() = default;
	Backpack(const Backpack&) = delete;
	Backpack(std::initializer_list<Item> list) : vec_(list) {}

	Backpack(Backpack&& bp) 
		: vec_(std::move(bp.vec_))	// need to move() again!
		// is actually lvalue ref within scope of this function
		// bp gives ownership to Backback, still Backpack needs give ownership to vec_
	{ 
		bp.vec_.clear(); 
	}

	// ----------------------------- perfect forwarding -------------------------------
	template <typename ...Args>
	void add(Args&& ...args)
	{
		vec_.emplace_back(std::forward<Args>(args)...);	// universal reference
	}

	void add(const Item& item) 
	{
		vec_.emplace_back(item); 
	}

	void remove(const Item& item)
	{
		// find needs operator==
		//auto pos = std::find(myVector.begin(), myVector.end(), 8);
		//if (pos != myVector.end())
		//	myVector.erase(position);
		vec_.erase(std::remove(vec_.begin(), vec_.end(), item));	// erase / delete idiom
	}
	
	void print()
	{
		for (const auto& i : vec_) { std::cout << i << ", "; }
		std::cout << std::endl;
		//std::copy(vec_.begin(), vec_.end(), std::ostream_iterator<std::string>(std::cout, ", "));
	}

private:
	std::vector<Item> vec_;
};

void TEST_backpack()
{
	Backpack bp{ { "knife", 1 },{ "axe", 2 },{ "pistol", 3 } };
	
	//bp.print();
	//bp.remove();
	{
		Backpack second(std::move(bp));
		second.print();
		bp.add("smg", 4);
		bp.print();
	}

	std::cin.get();
}

struct Crap
{
	Crap() = default;
	Crap(int i) : resource(new int(i)) {}
	Crap(const Crap& other)
	{
		if (other.resource != nullptr)
		{
			resource = new int(*other.resource);
		}
	}
	Crap& operator=(const Crap& other)
	{
		if (other.resource != nullptr)
		{
			resource = new int(*other.resource);
		}
		return *this;
	}
	Crap(Crap&& other)
	{
		if (other.resource != nullptr)
		{
			resource = other.resource;
			other.resource = nullptr;
		}
	}
	Crap& operator=(Crap&& other)
	{
		if (other.resource != nullptr)
		{
			resource = other.resource;
			other.resource = nullptr;
		}
		return *this;
	}
	~Crap()
	{
		if (resource != nullptr)
		{
			delete resource;
			resource = nullptr;
		}
	}
	int* resource = nullptr;
};

// lvalue
// rvalue
// prvalue -> pure rvalue
// xvalue -> expiring rvalue

int prvalue() { return 666; }

void TEST_crap()
{
	int i = 32;
	int j = prvalue();

	Crap crap;
	{
		Crap crap2(7);
		//crap2.resource = new int(33);
		crap = std::move(crap2);
		crap = Crap(33);
	}
	std::cout << *crap.resource << std::endl;
}

void TEST_PerfectFowarding()
{
	std::vector<int> v1;
	{
		std::vector<int> v2 = { 1, 2, 3, 4, 5, 6 };
		v1 = std::move(v2);
	}
}

int main(int argc, char** argv)
{
	TEST_PerfectFowarding();
	TEST_backpack();
	TEST_crap();

	return 0;
}
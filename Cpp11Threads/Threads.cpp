#include <iostream>
#include <thread>
#include <string>

using namespace std; 

void func(string& str, string* r)
{
	*r = str + "some shit";
	cout << "func(): " << str << endl;
}


class Func
{
public:
	string& s;
	string* r;
	Func(string& str, string* rstr) : s(str), r(rstr) {}

	void operator()() {
		*r = s + " form Func";
	}
};

int main(int argc, char** argv)
{
	auto f = [](const string& str, string* r) mutable { *r = str + " from lamba."; };
	
	string s = "this is a text";
	string r1, r2, r3;

	thread th1{ func, s, &r1 };
	thread th2{ Func(s, &r2) };
	thread th3{ []() { cout << "thread 3" << endl; } };
	thread th4{ f, s, &r3 };
	

	th1.join();
	th2.join();
	th3.join();
	th4.join();

	return 0;
}
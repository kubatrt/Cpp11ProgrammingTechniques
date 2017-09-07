#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <future>
#include <condition_variable>
#include <queue>
#include <utility>

using namespace std;

mutex g_mutex;

queue<char> g_recv_q;
condition_variable g_recv_cond;
mutex g_recv_mutex;

queue<char> g_send_q;
condition_variable g_send_cond;
mutex g_send_mutex;

void producer() {
	default_random_engine eng;
	uniform_int<char> chardist{ 'a', 'z' };
	while (true) {
		char c = chardist(eng);
		unique_lock<mutex> recv_lock{ g_recv_mutex };
		g_recv_q.push(c);
		g_recv_cond.notify_one();
		recv_lock.unlock();
		this_thread::sleep_for(chrono::milliseconds{ 100 });
	}
}

void modifier() {
	while (true) {
		unique_lock<mutex> recv_lock{ g_recv_mutex };
		while (g_recv_q.empty()) {
			g_recv_cond.wait(recv_lock);
		}
		char c = g_recv_q.front();
		g_recv_q.pop();
		recv_lock.unlock();
		
		c -= 32;	// convert to uppercase
		unique_lock<mutex> send_lock{ g_send_mutex };
		g_send_q.push(c);
		g_send_cond.notify_one();
	}
}

void consumer()
{
	while (true) {
		unique_lock<mutex> send_lock{ g_send_mutex };
		while (g_send_q.empty())
			g_send_cond.wait(send_lock);
		char c = g_send_q.front();
		g_send_q.pop();
		cout << c << endl;
	}
}

void mutexes()
{
	default_random_engine	eng;
	uniform_int<int> dist(0, 9);
	uniform_int<char> chardist('a', 'z');

	// 1. outputs 80 random characters from 'a' to 'z'
	auto rand_char = [&]() {
		g_mutex.lock();
		for (int i = 0; i < 80; ++i) {
			cout << chardist(eng);
		}
		cout << endl;
		g_mutex.unlock();
	};

	// 2. second lambda expression outputs 80 random digits between 0 to 9
	auto rand_digit = [&]() {
		g_mutex.lock();
		for (int i = 0; i < 80; ++i) {
			cout << dist(eng);
		}
		cout << endl;
		g_mutex.unlock();
	};


	thread t1{ rand_char };
	thread t2{ rand_digit };

	t1.join();
	t2.join();
}

void sharing_data()
{
	thread t1{ producer };
	thread t2{ modifier };
	thread t3{ consumer };
}


void consumer2(future<char>& fx)
{
	char c = fx.get();
	cout << c << endl;
}

void promise_future()
{
	promise<char> px;
	future<char> fx = px.get_future();

	thread t1{ consumer2, ref(fx) };
	default_random_engine	eng;
	uniform_int<char> chardist('a', 'z');

	px.set_value(chardist(eng));

	t1.join();
}

int func(int n) {
	return n;
}

void package_task()
{
	packaged_task<int(int)> f(func);
	future<int> r = f.get_future();

	thread t1(move(f), 9);
	cout << r.get() << endl;	// will block until func returns from the thread
	t1.join();
}

struct func_object {
	int n;
	func_object(int i) : n(i) {}
	int operator()() {
		return n;
	}

};

void async()
{
	future<int> f5 = async(func, 5);
	future<int> f10 = async(func_object(10));
	auto f20 = async([](int i) { return i; }, 20);

	cout << f5.get() << endl;
	cout << f10.get() << endl;
	cout << f20.get() << endl;
}

void properties_this_thread()
{
	auto f = []() {
		cout << "ID of this thread: " << this_thread::get_id() << endl;
		this_thread::yield();
		this_thread::sleep_for(chrono::milliseconds(500));
		this_thread::sleep_until(chrono::steady_clock::now() + chrono::milliseconds(5));
	};
	thread t1(f);
	t1.join();
}


int main()
{
	//mutexes();	
	//sharing_data();
	//promise_future();
	//package_task();
	async();
	properties_this_thread();

	return 0;
}
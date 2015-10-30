#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <exception>
#include <queue>
#include <memory>
#include <mutex>
#include <thread>
#include <future>
#include <atomic>
#include <utility>
#include <deque>
#include <list>
#include <condition_variable>

using namespace std;

class function_wrapper {
private:
	struct impl_base {
		virtual void call() = 0;
		virtual ~impl_base() {}
	};
	
	unique_ptr<impl_base> impl;
	
	template<typename F>
	struct impl_type: impl_base {
		F f;
		impl_type(F&& f_): f(std::move(f_)) {}
		void call() {f();}
	};
public:
	template<typename F>
	function_wrapper(F&& f):
		impl(new impl_type<F>(move(f))) 
	{}
	
	void operator()() {impl->call();}
	
	function_wrapper() = default;

	function_wrapper(function_wrapper&& other):
		impl(move(other.impl))
	{}

	function_wrapper& operator=(function_wrapper&& other) {
		impl = move(other.impl);
		return *this;
	}

	function_wrapper(const function_wrapper&) = delete;

	function_wrapper(function_wrapper&) = delete;

	function_wrapper& operator=(const function_wrapper&) = delete;
};

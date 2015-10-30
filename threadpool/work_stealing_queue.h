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

class work_stealing_queue {
private:
	typedef function_wrapper type;
	deque<type> que;
	mutable mutex mut;
public:
	work_stealing_queue() {}

	work_stealing_queue(const work_stealing_queue& other) = delete;

	work_stealing_queue& operator=(const work_stealing_queue& other) = delete;

	void push(type v) {
		lock_guard<mutex> lock(mutex);
		que.push_front(move(v));
	}

	bool empty() const {
		lock_guard<mutex> lock(mutex);
		return que.empty();
	}

	bool try_pop(type& res) {
		lock_guard<mutex> lock(mutex);
		if(que.empty()) return false;
		res = move(que.front());
		que.pop_front();
		return true;	
	}

	bool try_steal(type& res) {
		lock_guard<mutex> lock(mutex);
		if (que.empty()) return false;
		res = move(que.back());
		que.pop_back();
		return true;
	}
};
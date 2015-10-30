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

class join_threads {
private:
	vector<thread>& threads;
public:
	explicit join_threads(vector<thread>& threads_):
		threads(threads_){}
	~join_threads() {
		for (unsigned long i = 0; i < threads.size(); ++i) {
			if (threads[i].joinable())
				threads[i].join();
		}
	}
};
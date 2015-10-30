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

template <typename T>
class threadsafe_queue {
private:
	struct node {
		shared_ptr<T> data;
		unique_ptr<node> next;
	};
	mutex head_mutex;
	unique_ptr<node> head;
	mutex tail_mutex;
	node *tail;
	condition_variable data_cond;

	node* get_tail() {
		lock_guard<mutex> tail_lock(tail_mutex);
		return tail;
	}

	unique_lock<mutex> wait_for_data() {
		unique_lock<mutex> head_lock(head_mutex);
		data_cond.wait(head_lock, [&]{return head.get() != get_tail();});
		return move(head_lock);
	}

	unique_ptr<node> pop_head() {
		unique_ptr<node> old_head = move(head);
		head = move(old_head->next);
		return old_head;
	}

	unique_ptr<node> wait_pop_head() {
		unique_lock<mutex> head_lock(wait_for_data());
		return pop_head();
	}

	unique_ptr<node> wait_pop_head(T& v) {
		unique_lock<mutex> head_lock(wait_for_data());
		v = move(*head->data);
		return pop_head();
	}

	unique_ptr<node> try_pop_head() {
		lock_guard<mutex> head_lock(head_mutex);
		if (head.get() == get_tail()) {
			return unique_ptr<node>();
		}
		return pop_head();
	}

	unique_ptr<node> try_pop_head(T& v) {
		lock_guard<mutex> head_lock(head_mutex);
		if (head.get() == get_tail()) return unique_ptr<node>();
		v = move(*head->data);
		return pop_head();
	}
public:
	threadsafe_queue(): head(new node), tail(head.get()) {}

	threadsafe_queue(const threadsafe_queue& other) = delete;

	threadsafe_queue& operator = (const threadsafe_queue& other) = delete;

	void push(T v) {
		shared_ptr<T> new_data (make_shared<T>(move(v)));
		unique_ptr<node> p(new node);
		lock_guard<mutex> tail_lock(tail_mutex);
		tail->data = new_data;
		node* const new_tail = p.get();
		tail->next = move(p);
		tail = new_tail;
		data_cond.notify_one();
	}

	bool empty() {
		lock_guard<mutex> head_lock(head_mutex);
		return (head.get() == get_tail());
	}

	shared_ptr<T> wait_and_pop() {
		unique_ptr<node> const old_head = wait_pop_head();
		return old_head->data;
	}

	void wait_and_pop(T& v) {
		unique_ptr<node> const old_head = wait_pop_head(v);
	}

	shared_ptr<T> try_pop() {
		unique_ptr<node> old_head = try_pop_head();
		return old_head?old_head->data:shared_ptr<T>();
	}

	bool try_pop(T& v) {
		unique_ptr<node> const old_head = try_pop_head(v);
		return old_head?true:false;
	}
};

#include "threadsafe_queue.h"
#include "function_wrapper.h"
#include "join_threads.h"
#include "work_stealing_queue.h"

using namespace std;

class thread_pool {
private:
	typedef function_wrapper task_type;
	atomic_bool done;
	threadsafe_queue<task_type> pool_work_queue;
	vector<unique_ptr<work_stealing_queue> > queues;
	vector<thread> threads;
	join_threads joiner;

	static thread_local work_stealing_queue* local_work_queue;
	static thread_local unsigned int my_index;

	void worker_thread(unsigned int my_index_);

	bool pop_task_from_local_queue(task_type& task);

	bool pop_task_from_pool_queue(task_type& task);

	bool pop_task_from_other_thread_queue(task_type& task);

public:
	thread_pool(): done(false), joiner(threads) {
		local_work_queue = NULL;
		my_index = 0;
		unsigned int const thread_count = thread::hardware_concurrency();
		try {
			for(unsigned int i = 0; i < thread_count; ++i) {
				queues.push_back(unique_ptr<work_stealing_queue> (new work_stealing_queue));
				threads.push_back(thread(&thread_pool::worker_thread, this, i));
			}
		} catch (...) {
			done = true;
			throw;
		}
	}

	~thread_pool() {done = true;}

	void run_pending_task();

	template<typename F> 
	future<typename result_of<F()>::type> submit(F f) {
		typedef typename result_of<F()>::type result_type;
		packaged_task<result_type()> task(move(f));
		future<result_type> res(task.get_future());

		if (local_work_queue) {
			local_work_queue->push(move(task));
		} else {
			pool_work_queue.push(move(task));
		}
		return res;
	}
};




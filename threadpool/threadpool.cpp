#include "threadpool.h"

thread_local work_stealing_queue* thread_pool::local_work_queue = NULL;
thread_local unsigned int thread_pool::my_index = 0;

void thread_pool::run_pending_task() {
	task_type task;
	if (pop_task_from_local_queue(task) ||
		pop_task_from_pool_queue(task) || 
		pop_task_from_other_thread_queue(task)) {
		
		task();
	} else {
		this_thread::yield();
	}
}

void thread_pool::worker_thread(unsigned int my_index_) {
	my_index = my_index_;
	local_work_queue = queues[my_index].get();
	while(!done) {
		run_pending_task();
	} 
}

bool thread_pool::pop_task_from_local_queue(task_type& task) {
	return local_work_queue && local_work_queue->try_pop(task);
}

bool thread_pool::pop_task_from_pool_queue(task_type& task) {
	return pool_work_queue.try_pop(task);
}

bool thread_pool::pop_task_from_other_thread_queue(task_type& task) {
	for (unsigned int i = 0; i < queues.size(); ++i) {
		unsigned int const index = (my_index + i + 1) % queues.size();
		if (queues[index]->try_steal(task)) return true;
	}
	return false;
}


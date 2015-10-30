#include "threadpool.h"

using namespace std;

#define MAXN 20

struct Test1{
	thread_pool pool;
	bool test() {
		cout << "Thread id: " << this_thread::get_id()<< endl;
	}

	void testthread() {
		vector<future<bool> > futures(MAXN);
		for (int i = 0; i < MAXN; i++) {
			futures[i] = pool.submit(bind(&Test::test, this));
		}
		for (int i = 0; i < MAXN; i++) {
			futures[i].get();
		}
	}
};

int main(int argc, char *argv[]) {
	Test1 t;
	t.testthread();
	exit(0);
}

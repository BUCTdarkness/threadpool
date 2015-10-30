objects = main.o threadpool.o
main: $(objects)
	g++ -o main $(objects) -pthread -lapue -lpthread 

threadpool.o: threadpool.h threadsafe_queue.h function_wrapper.h \
	join_threads.h work_stealing_queue.h threadpool.cpp
	g++ -c threadpool.cpp -std=c++11

main.o: threadpool.h main.cpp
	g++ -c main.cpp -std=c++11
clean:
	rm main $(objects)
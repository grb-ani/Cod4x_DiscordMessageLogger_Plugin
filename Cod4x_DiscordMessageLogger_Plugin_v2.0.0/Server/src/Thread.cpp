#include "Thread.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

void Thread::setThreadFunc(void* (*threadFunc) (void*)) {
	tFunc = threadFunc;

}

void Thread::run(void* arg) {
	rValue = pthread_create(&thread, nullptr, tFunc, arg);
	if(rValue != 0) {
		fprintf(stderr, "Error occurred on creating thread, exiting...\n");
		exit(EXIT_FAILURE);

	}

	pthread_detach(thread);

}

void Thread::stop() {
	printf("NOTICE: Canceling thread...\n");
	pthread_cancel(thread);

}

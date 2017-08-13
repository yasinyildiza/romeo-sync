#include <iostream>

#include "Thread.h"

using namespace std;

void* start(void *data)
{
	Thread* thread = (Thread*)data;
	thread->run();
	return NULL;
}

Thread::Thread(){}

bool Thread::execute()
{
	return pthread_create(&thread, NULL, start, (void*)this) < 0;
}

bool Thread::cancel()
{
	return pthread_cancel(thread) < 0;
}

bool Thread::join()
{
	return pthread_join(thread, NULL) < 0;
}

bool Thread::detach()
{
	return pthread_detach(thread) < 0;
}

void Thread::exit()
{
	pthread_exit(NULL);
}

void Thread::stop()
{
	//detach();
	cancel();
	//join();
}

Thread::~Thread(){}

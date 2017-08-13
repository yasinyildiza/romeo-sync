#ifndef _THREAD_H
#define _THREAD_H

#include <pthread.h>

class Thread
{
	private:
		pthread_t thread;
		friend void* start(void*);
	public:
		Thread();
		bool execute();
		bool join();
		bool cancel();
		bool detach();
		void exit();
		void stop();
		virtual void run() = 0;
		virtual ~Thread();
};

#endif

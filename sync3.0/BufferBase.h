#ifndef _BUFFER_BASE_H
#define _BUFFER_BASE_H

#include <iostream>
#include <stdio.h>
#include "BufferNode.h"
#include "BufferBase.h"
#include "debugMessagePrinter.h"

typedef enum buffer_status
{
	buffer_empty,
	buffer_underflow,
	buffer_ok,
	buffer_overflow,
	buffer_full
} buffer_status_t;

template <class T>
class BufferBase
{
	protected:

		std::string buffer_id;
		BufferNode<T> *first;
		BufferNode<T> *last;
		int current_size;
		int total_size;
		int buffer_lower_threshold_percentage;
		int buffer_upper_threshold_percentage;
		pthread_mutex_t mutex;

		void initialize(std::string _buffer_id, int _total_size, int _buffer_lower_threshold_percentage, int _buffer_upper_threshold_percentage)
		{
			buffer_id = _buffer_id;
			total_size = _total_size;
			buffer_lower_threshold_percentage = _buffer_lower_threshold_percentage;
			buffer_upper_threshold_percentage = _buffer_upper_threshold_percentage;
			first = NULL;
			last = NULL;
			current_size = 0;
			mutex = PTHREAD_MUTEX_INITIALIZER;
			//printDebugMessage("buffer " + buffer_id + " created", STATUS);
		}
		void lock()
		{
			pthread_mutex_lock(&mutex);
		}
		void release()
		{
			pthread_mutex_unlock(&mutex);
		}
		const bool isEmpty()
		{
			return current_size == 0;
		}
		const bool isUnderflow()
		{
			return getFillRatio() < buffer_lower_threshold_percentage && !isEmpty();
		}
		const bool isOverflow()
		{
			return getFillRatio() > buffer_upper_threshold_percentage && !isFull();
		}
		const bool isFull()
		{
			return current_size == total_size;
		}
		void insertTail(T *packet)
		{
			lock();
			last->insertAfter(new BufferNode<T>(packet));
			last = last->next;
			insertSuccessful(packet);
		}
		void insertSuccessful(T *packet)
		{
			current_size++;
			//writeDebugMessage(buffer_id + "_insert.txt", now_str() + "\t" + to_str(getFillRatio()) + "%\t" + getStatusStr(), INFO);
			release();
		}
		void insertFailed(T *packet)
		{
			delete packet;
			release();
		}
		void emptyBuffer()
		{
			while(!isEmpty())
				dropFirst();
		}

	public:
		BufferBase()
		{
			buffer_id = "NO_ID";
			total_size = 100;
			buffer_lower_threshold_percentage = 10;
			buffer_upper_threshold_percentage = 90;
			first = NULL;
			last = NULL;
			current_size = 0;
			mutex = PTHREAD_MUTEX_INITIALIZER;
		}
		const buffer_status_t getStatus()
		{
			if(isEmpty())
				return buffer_empty;
			if(isUnderflow())
				return buffer_underflow;
			if(isOverflow())
				return buffer_overflow;
			if(isFull())
				return buffer_full;
			return buffer_ok;
		}
		const std::string getStatusStr()
		{
			if(isEmpty())
				return "empty";
			if(isUnderflow())
				return "underflow";
			if(isOverflow())
				return "overflow";
			if(isFull())
				return "full";
			return "ok";
		}
		const void printStatus()
		{
			writeDebugMessage(buffer_id + "_status.txt", now_str() + "\t" + getStatusStr(), STATUS);
		}
		const double getFillRatio()
		{
			return ((double)current_size * 100) / (double)total_size;
		}
		const void printFillRatio()
		{
			writeDebugMessage(buffer_id + "_fill_ratio.txt", now_str() + "\t" + to_str(getFillRatio()) + "%\t" + getStatusStr(), STATUS);
		}
		T *getFirst()
		{
			if(first != NULL)
			{
				//writeDebugMessage(buffer_id + "_send.txt", now_str() + "\t" + to_str(getFillRatio()) + "%\t" + getStatusStr(), INFO);
				return first->packet;
			}
			return NULL;
		}
		void dropFirst()
		{
			if(!isEmpty())
			{
				lock();
				BufferNode<T> *temp = first;
				first = first->next;
				delete temp;
				current_size--;
				release();
			}
		}
		void insert(T *packet)
		{
			if(packet != NULL)
			{
				if(isEmpty())
				{
					lock();
					first = new BufferNode<T>(packet);
					last = first;
					insertSuccessful(packet);
				}
				else if(isFull())
				{
					printStatus();
					insertFailed(packet);
				}
				else
				{
					insertNewNode(packet);
				}
			}
		}
		
		bool waitForNewPacket(int timeout)
		{
			int counter = 0;
			while(true)
			{
				usleep(1 * 1000 * 1000);
				counter++;
				if(!isEmpty())
					return true;
				if(counter >= timeout)
					return false;
			}
		}
			
		virtual void insertNewNode(T *packet) = 0;
		virtual ~BufferBase()
		{
			emptyBuffer();
		}
};

#endif

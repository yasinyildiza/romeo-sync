#ifndef _BUFFER_NODE_H
#define _BUFFER_NODE_H

template <class T>
class BufferNode
{
	private:
	public:
		T *packet;
		BufferNode *next;

		BufferNode()
		{
			packet = NULL;
			next = NULL;
		}
		BufferNode(T *_packet)
		{
			packet = _packet;
			next = NULL;
		}
		void insertAfter(BufferNode *node)
		{
			node->next = next;
			next = node;
		}
		BufferNode *deleteAfter()
		{
			if(next != NULL)
			{
				BufferNode *temp=next;
				next = temp->next;
				return temp;
			}
			return NULL;
		}
		~BufferNode()
		{
			delete packet;
		}
};

#endif
#include <queue>

#ifndef CIRCULARQUEUE
#define CIRCULARQUEUE

template<class T>
class CircularQueue
{
	private:
		std::queue<T> queue;
		int max;

	public:
		CircularQueue<T>(int max)
		{
			this->queue = std::queue<T>();
			this->max = max;
		}

		void push(T elem)
		{
			this->queue.push(elem);

			if(this->queue.size() > max)
			{
				this->queue.pop();
			}
		}

		T pop()
		{
			return this->queue.front();
		}
};

#endif

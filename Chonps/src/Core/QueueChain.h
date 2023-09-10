#ifndef HG_CHONPS_QUEUE_CHAIN_H
#define HG_CHONPS_QUEUE_CHAIN_H

#include <queue>
#include <set>

namespace Chonps
{
	// QueueChain class: The purpose of this class is to create a queue-like structure that contains only unique elements in an unsigned integer format
	//	Notes:
	//		- Queue only contains unique elements, No numbers will repeat in queue
	//		- Queue only accepts unsigned interger formats (e.g. uint8_t, uint16_t, uint32_t, uint64_t, size_t),
	//		  All other types will result in compile error
	//		- The main use of this is to store and keep track of individual object IDs, an object can take an ID from the queue
	//		  once the object has finished its purpose or is about to be deleted, the object ID is pushed back into the queue
	//		  to be taken again by another future object.
	template <typename T>
	class QueueChain
	{
	public:
		static_assert(std::is_same<T, uint8_t>::value || 
			std::is_same<T, uint16_t>::value ||
			std::is_same<T, uint32_t>::value ||
			std::is_same<T, uint64_t>::value || 
			std::is_same<T, size_t>::value,
			"queue type error, queue type must be in unsigned integer format");

		QueueChain() = default;
		QueueChain(T firstElement, T maxElement)
			: m_Next(maxElement + 1)
		{
			for (T i = firstElement; i <= maxElement; i++)
			{
				m_Queue.push(i);
				m_Available.insert(i);
			}
		}

		// Pushes an unsigned integer into the queue, Element must not already exist in the queue
		// If the element given already exists within the queue function will return false, otherwise, true.
		bool push(T element)
		{
			if (m_Available.find(element) == m_Available.end())
			{
				m_Queue.push(element);
				m_Available.insert(element);
				return true;
			}

			return false;
		}

		void pop()
		{
			if (m_Queue.empty())
				return;

			m_Available.erase(m_Queue.front());
			m_Queue.pop();
		}

		// Takes the next element from the queue
		T take_next()
		{
			if (m_Queue.empty())
			{
				m_Queue.push(m_Next);
				m_Next += 1;
			}
			T front = m_Queue.front();
			m_Available.erase(front);
			m_Queue.pop();
			return front;
		}

		// Takes in the number of times to push new elements to the queue starting from the next element value
		void push_elements(T numElements)
		{
			for (T i = 0; i < numElements; i++)
			{
				if (m_Available.find(m_Next) == m_Available.end())
				{
					m_Available.insert(m_Next);
					m_Queue.push(m_Next);
				}
				m_Next++;
			}
		}

		size_t size() { return m_Queue.size(); }
		T front() { return m_Queue.front(); }
		T back() { return m_Queue.back(); }
		bool empty() { return m_Queue.empty(); }

		std::queue<T>& getQueue() { return m_Queue; }
		std::set<T>& getAvailable() { return m_Available; }
		T getnext() { return m_Next; }

	private:
		std::queue<T> m_Queue;
		std::set<T> m_Available;

		T m_Next = 0;
	};
}

#endif

#ifndef CHONPS_THREAD_POOL_H
#define CHONPS_THREAD_POOL_H

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>

namespace Chonps
{
	class Thread
	{
	public:
		Thread()
		{
			m_Worker = std::thread(&Thread::queueLoop, this);
		}

		~Thread()
		{
			if (m_Worker.joinable())
			{
				wait();
				m_QueueMutex.lock();
				m_Destroyed = true;
				m_Condition.notify_one();
				m_QueueMutex.unlock();
				m_Worker.join();
			}
		}

		// Add a new job to the thread's queue
		void addJob(std::function<void()> function)
		{
			std::lock_guard<std::mutex> lock(m_QueueMutex);
			m_JobQueue.push(std::move(function));
			m_Condition.notify_one();
		}

		// Wait until all work items have been finished
		void wait()
		{
			std::unique_lock<std::mutex> lock(m_QueueMutex);
			m_Condition.wait(lock, [this]() { return m_JobQueue.empty(); });
		}

	private:
		bool m_Destroyed = false;
		std::thread m_Worker;
		std::queue<std::function<void()>> m_JobQueue;
		std::mutex m_QueueMutex;
		std::condition_variable m_Condition;

		// Loop through all remaining jobs
		void queueLoop()
		{
			while (true)
			{
				std::function<void()> job;
				{
					std::unique_lock<std::mutex> lock(m_QueueMutex);
					m_Condition.wait(lock, [this] { return !m_JobQueue.empty() || m_Destroyed; });
					if (m_Destroyed)
					{
						break;
					}
					job = m_JobQueue.front();
				}

				job();

				{
					std::lock_guard<std::mutex> lock(m_QueueMutex);
					m_JobQueue.pop();
					m_Condition.notify_one();
				}
			}
		}

	};

	class ThreadPool
	{
	public:
		std::vector<std::unique_ptr<Thread>> threads;

		// Sets the number of threads to be allocated in this pool
		void setThreadCount(uint32_t count)
		{
			threads.clear();
			for (uint32_t i = 0; i < count; i++)
			{
				threads.push_back(std::make_unique<Thread>());
			}
		}

		// Wait until all threads have finished their work items
		void wait()
		{
			for (auto& thread : threads)
			{
				thread->wait();
			}
		}
	};
}

#endif
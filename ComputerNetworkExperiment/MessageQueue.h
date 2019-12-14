#pragma once

#include <mutex>
#include <queue>
#include <functional>
#include <future>
#include <thread>
#include <utility>
#include <vector>
#include <functional>
#include "SMTPClient.h"

template <typename T>
class Queue {
private:
	std::queue<T> queue;
	std::mutex m;
public:
	Queue() {}
		
	~Queue() {}
		
	bool empty()
	{
		std::unique_lock<std::mutex> lock(m);
		return queue.empty();
	}

	int size()
	{
		std::unique_lock<std::mutex> lock(m);
		return queue.size();
	}
		
	void enqueue(T& t)
	{
		std::unique_lock<std::mutex> lock(m);
		queue.push(t);
	}

	bool dequeue(T& t)
	{
		std::unique_lock<std::mutex> lock(m);
		if (queue.empty())
		{
			return false;
		}
		t = std::move(queue.front());
		queue.pop();
		return true;
	}

};


class MessageQueue
{
private:
	class Worker
	{
	private:
		int id;
		MessageQueue* mq;
	public:
		Worker(MessageQueue* mq, const int id) : mq(mq), id(id) {}
		void operator()()
		{
			std::function<void()> func;
			bool dequeued;
			while (!mq->_shutdown)
			{
				std::unique_lock<std::mutex> lock(mq->condititonal_mutex);
				if (mq->queue.empty())
				{
					mq->cv.wait(lock);
				}
				dequeued = mq->queue.dequeue(func); // get task
				if (dequeued)
				{
					func();
				}
			}
		}
	};

	bool _shutdown;
	Queue<std::function<void()>> queue;
	std::vector<std::thread> threads;
	std::mutex condititonal_mutex;
	std::condition_variable cv;
public:
	MessageQueue(const int n) : threads(std::vector<std::thread>(n)), _shutdown(false) {}
	
	MessageQueue(const MessageQueue&) = delete; // not allowed to use
	MessageQueue(MessageQueue&&) = delete;

	MessageQueue& operator=(const MessageQueue&) = delete;
	MessageQueue& operator=(MessageQueue&&) = delete;

	void init()
	{
		int size = threads.size();
		for (int i = 0; i < size; ++i)
		{
			threads[i] = std::thread(Worker(this, i));
		}
	}

	// Waits until threads finish their current task and shutdowns the pool
	void shutdown()
	{
		_shutdown = true;
		cv.notify_all();

		int size = threads.size();
		for (int i = 0; i < size; ++i)
		{
			if (threads[i].joinable())
			{
				threads[i].join();
			}
		}
	}

	template<typename F, typename...Args>
	auto submitFunc(F&& f, Args&&... args) -> std::future<decltype(f(args...))> 
	{
		std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
		std::function<void()> wrapper_func = [task_ptr]()
		{
			(*task_ptr)();
		};

		queue.enqueue(wrapper_func);
		cv.notify_one();
		return task_ptr->get_future();
	}

	std::future<bool> SendMail(SMTPClient client, Mail mail);

};

std::future<bool> MessageQueue::SendMail(SMTPClient client, Mail mail)
{
	return submitFunc(std::bind(&SMTPClient::SendMail, &client, mail));
}

template<typename R>
bool is_ready(std::future<R> const& f)
{
	return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}
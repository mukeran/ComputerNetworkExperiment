#pragma once

#include <mutex>
#include <queue>
#include <functional>
#include <future>
#include <thread>
#include <utility>
#include <vector>
#include <functional>
#include "smtp/client.h"

template <typename T>
class lock_queue
{
	std::queue<T> queue_;
	std::mutex m_;
public:
	lock_queue() = default;
	~lock_queue() = default;
	
	bool empty()
	{
		std::unique_lock<std::mutex> lock(m_);
		return queue_.empty();
	}

	int size()
	{
		std::unique_lock<std::mutex> lock(m_);
		return queue_.size();
	}
		
	void enqueue(T& t)
	{
		std::unique_lock<std::mutex> lock(m_);
		queue_.push(t);
	}

	bool dequeue(T& t)
	{
		std::unique_lock<std::mutex> lock(m_);
		if (queue_.empty())
		{
			return false;
		}
		t = std::move(queue_.front());
		queue_.pop();
		return true;
	}

};


class message_queue
{
	class worker
	{
		int id_;
		message_queue* mq_;
	public:
		worker(message_queue* mq, const int id) : id_(id), mq_(mq) {}
		void operator()() const
		{
			logger::debug("Worker " + std::to_string(id_) + " created!");
			std::function<void()> func;
			while (!mq_->shutdown_)
			{
				std::unique_lock<std::mutex> lock(mq_->conditional_mutex_);

				while (mq_->queue_.empty())
				{
					//logger::debug("Worker" + std::to_string(id_) + " get to sleep");
					mq_->cv_.wait(lock);// The wait operations atomically release the mutex and suspend the execution of the thread.
					// the mutex is atomically reacquired.
					//logger::debug("Worker" + std::to_string(id_) + " waked");
				}
				logger::debug("Worker" + std::to_string(id_) + "get a task.");
				const auto dequeued = mq_->queue_.dequeue(func); // get task
				if (dequeued)
				{
					func();
				}
				//logger::debug("Worker" + std::to_string(id_) + " finished task");
			}
		}
	};

	bool shutdown_;
	lock_queue<std::function<void()>> queue_;
	std::vector<std::thread> threads_;
	std::mutex conditional_mutex_;
	std::condition_variable cv_;
public:
	static message_queue* instance;
	explicit message_queue(const int n) : shutdown_(false), threads_(std::vector<std::thread>(n)) {}
	
	message_queue(const message_queue&) = delete; // not allowed to use
	message_queue(message_queue&&) = delete;

	message_queue& operator=(const message_queue&) = delete;
	message_queue& operator=(message_queue&&) = delete;

	~message_queue()
	{
		shutdown();
	}

	void init()
	{
		const int size = threads_.size();
		for (auto i = 0; i < size; ++i)
		{
			threads_[i] = std::thread(worker(this, i));
			auto thread_id = threads_[i].get_id();
			//logger::debug("create a new thread: " + i);
		}
	}

	// Waits until threads finish their current task and shutdowns the pool
	void shutdown()
	{
		shutdown_ = true;
		cv_.notify_all();

		const int size = threads_.size();
		for (auto i = 0; i < size; ++i)
		{
			logger::debug("joining thread: " + std::to_string(i) +"which is: " + std::to_string(threads_[i].joinable()));
			if (threads_[i].joinable())
			{
				threads_[i].join();
			}
		}
	}

	template<typename F, typename...Args>
	auto submit_func(F&& f, Args&&... args) -> std::future<decltype(f(args...))> 
	{
		std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
		std::function<void()> wrapper_func = [task_ptr]()
		{
			(*task_ptr)();
		};
		queue_.enqueue(wrapper_func);
		std::lock_guard<std::mutex> lock(instance->conditional_mutex_);
		cv_.notify_one();
		return task_ptr->get_future();
	}

	std::future<bool> send_mail(mail mail, const smtp::auth& auth)
	{
		logger::info("Mail " + mail.uuid + " is pushing into message queue");
		return submit_func([mail, auth]()
		{
			auto m = mail;
			return smtp::client::instance->send(&m, auth);
		});
	}
};

template<typename R>
bool is_ready(std::future<R> const& f)
{
	return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

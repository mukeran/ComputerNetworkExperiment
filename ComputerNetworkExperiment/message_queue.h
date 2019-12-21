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
			std::function<void()> func;
			while (true)
			{
				{
					std::unique_lock<std::mutex> lock(mq_->queue_mutex_);
					//logger::debug("Worker" + std::to_string(id_) + " get to sleep");
					mq_->cv_.wait(lock, [this] { return mq_->shutdown_ || !mq_->queue_.empty(); });// The wait operations atomically release the mutex and suspend the execution of the thread.
					// the mutex is atomically reacquired.
					//logger::debug("Worker" + std::to_string(id_) + " waked");
					if (mq_->shutdown_ && mq_->queue_.empty())
					{
						return;
					}
					func = std::move(mq_->queue_.front()); // get task
					mq_->queue_.pop();
					logger::debug("Worker " + std::to_string(id_) + " get a task.");
				} // in order to make lock out of scope and release
				
				func();
				//logger::debug("Worker" + std::to_string(id_) + " finished task");
			}
		}
	};

	bool shutdown_;
	std::queue< std::function<void()> > queue_;
	std::vector<std::thread> threads_;
	std::mutex queue_mutex_;
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
		{
			std::unique_lock<std::mutex> lock(queue_mutex_);
			shutdown_ = true;
		}
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
		{
			std::unique_lock<std::mutex> lock(queue_mutex_);
			if (shutdown_)
				throw std::runtime_error("enqueue on stop");
			queue_.emplace(wrapper_func);
		}
		logger::info("Mail pushed !!");
		cv_.notify_one();
		return task_ptr->get_future();
	}

	std::future<bool> send_mail(const mail& mail, const smtp::auth& auth)
	{
		logger::info("Mail " + mail.uuid + " is pushing into message queue");
		return submit_func([mail, auth]()
		{
			std::this_thread::sleep_for(std::chrono::seconds(5));
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

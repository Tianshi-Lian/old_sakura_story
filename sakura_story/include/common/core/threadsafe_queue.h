#pragma once

#include "common/common.h"

namespace sakura::common::core {

	template<typename T>
	class Threadsafe_Queue {
	public:
		Threadsafe_Queue() = default;
		Threadsafe_Queue(const Threadsafe_Queue<T>&) = delete;
		virtual ~Threadsafe_Queue() { clear(); }

	public:
		const T& front() {
			std::scoped_lock lock(m_mutex);
			return m_queue.front();
		}

		const T& back() {
			std::scoped_lock lock(m_mutex);
			return m_queue.back();
		}

		T popFront() {
			std::scoped_lock lock(m_mutex);
			auto t = std::move(m_queue.front());
			m_queue.pop_front();
			return t;
		}

		T popBack() {
			std::scoped_lock lock(m_mutex);
			auto t = std::move(m_queue.back());
			m_queue.pop_back();
			return t;
		}

		void pushBack(const T& item) {
			std::scoped_lock lock(m_mutex);
			m_queue.emplace_back(std::move(item));

			std::unique_lock ul(m_mutexBlock);
			m_cvBlock.notify_one();
		}

		void pushFront(const T& item) {
			std::scoped_lock lock(m_mutex);
			m_queue.emplace_front(std::move(item));

			std::unique_lock ul(m_mutexBlock);
			m_cvBlock.notify_one();
		}

		bool isEmpty() {
			std::scoped_lock lock(m_mutex);
			return m_queue.empty();
		}

		size_t size() {
			std::scoped_lock lock(m_mutex);
			return m_queue.size();
		}

		void clear() {
			std::scoped_lock lock(m_mutex);
			m_queue.clear();
		}

		void wait() {
			while (isEmpty()) {
				std::unique_lock ul(m_mutexBlock);
				m_cvBlock.wait(ul);
			}
		}

	protected:
		std::mutex m_mutex;
		std::deque<T> m_queue;

		std::condition_variable m_cvBlock;
		std::mutex m_mutexBlock;
	};

}

#pragma once

#include "common/common.h"

namespace sakura::common::core {

	template <typename T>
	class Threadsafe_Queue {
	public:
		Threadsafe_Queue() = default;
		Threadsafe_Queue(const Threadsafe_Queue&) = delete;

		virtual ~Threadsafe_Queue() {
			clear();
		}

		const T& front() {
			std::scoped_lock lock(m_mutex);
			return m_queue.front();
		}

		void pushBack(const T& item) {
			std::scoped_lock lock(m_mutex);
			m_queue.push_back(item);
		}

		void pushFront(const T& item) {
			std::scoped_lock lock(m_mutex);
			m_queue.push_front(item);
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

		void clear() {
			std::scoped_lock lock(m_mutex);
			m_queue.clear();
		}

		[[nodiscard]] bool isEmpty() const {
			std::scoped_lock lock(m_mutex);
			return m_queue.empty();
		}

		[[nodiscard]] size_t size() const {
			std::scoped_lock lock(m_mutex);
			return m_queue.size();
		}

	private:
		std::mutex m_mutex;
		std::deque<T> m_queue;
	};
	
}

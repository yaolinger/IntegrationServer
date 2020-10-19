#ifndef UTILS_THREAD_POOL_HPP
#define UTILS_THREAD_POOL_HPP

#include <vector>

#include "base_thread.hpp"

NS_UTILS_BEGIN

// 线程池
class ThreadPool : public Noncopyable {
public:
	ThreadPool() {
	}
	~ThreadPool() {
		reset();
	}

public:
	template <typename Func>
	void createThread(Func f, uint32 thread_num) {
		for (uint32 i = 0; i < thread_num; i++) {
			BaseThreadPtr ptr = std::make_shared<BaseThread>(f);
			m_thread_vec.push_back(ptr);
		}
	}

	void reset() {
        m_thread_vec.clear();
    }

	void join() {
		for (auto & iter : m_thread_vec) {
			if (NULL!= iter) {
				iter->join();
			}
		}
	}

private:
	std::vector<BaseThreadPtr> m_thread_vec;
};

NS_UTILS_END

#endif

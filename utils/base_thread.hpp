#ifndef UTILS_BASE_THREAD_HPP
#define UTILS_BASE_THREAD_HPP

#include <thread>
#include <memory>

#include "macros.hpp"
#include "noncopyable.hpp"

NS_UTILS_BEGIN

class BaseThread : public Noncopyable {
public:
    template<typename Func>
    BaseThread(Func f) : m_thread(f) {
    }

    ~BaseThread() {
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    void join() {
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

private:
    std::thread m_thread; //标准库线程对象
};

typedef std::shared_ptr<BaseThread> BaseThreadPtr;

NS_UTILS_END

#endif

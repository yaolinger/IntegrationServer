#ifndef UTILS_SEMAPHORE_HPP
#define UTILS_SEMAPHORE_HPP

#include "macros.hpp"
#include "noncopyable.hpp"
#include <semaphore.h>

NS_UTILS_BEGIN

// 信号量封装
// 暂时用于线程间串行
// 暂时不处理返回值
class Semaphore : public Noncopyable {
public:
	Semaphore(uint32 count = 0);
	~Semaphore();

public:
	// 阻塞等待信号
	void wait();
	// 信号通知
	void notify();

private:
	sem_t mSemaphore; // 基础信号量
};

NS_UTILS_END

#endif

#include "semaphore.hpp"

NS_UTILS_BEGIN

Semaphore::Semaphore(uint32 count) {
	sem_init(&mSemaphore, 0, count);
}

Semaphore::~Semaphore() {
	sem_destroy(&mSemaphore);
}

void Semaphore::wait() {
	sem_wait(&mSemaphore);
}

void Semaphore::notify() {
	sem_post(&mSemaphore);
}

NS_UTILS_END

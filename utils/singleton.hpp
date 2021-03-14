#ifndef UTILS_SINGLETON_HPP
#define UTILS_SINGLETON_HPP

#include <memory>
#include "macros.hpp"

NS_UTILS_BEGIN

// 获取模板对象指针
template <typename T>
class Singleton {
public:
	static T* getInstance() {
		static T v;
		return &v;
	}
	Singleton(const Singleton<T>& data) = delete; 
	Singleton(Singleton<T>&& data) = delete;

protected:
	Singleton() {}
};

// 获取模板对象智能指针
template <typename T>
class SingletonPtr {
public:
	static std::shared_ptr<T> getInstancePtr() {
		//static std::shared_ptr<T> ptr = std::make_shared<T>(); 不可以使用make_shared
		static std::shared_ptr<T> ptr(new T);
		return ptr;
	}
	SingletonPtr(const Singleton<T>& data) = delete; 
	SingletonPtr(Singleton<T>&& data) = delete;

protected:
	SingletonPtr() {}
};


NS_UTILS_END

#endif



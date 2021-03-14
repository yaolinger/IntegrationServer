#ifndef TEST_UTILS_TEST_SINGLETON_HPP
#define TEST_UTILS_TEST_SINGLETON_HPP

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"
#include "utils/singleton.hpp"

void TestSingleton() {
    FUNC_TRACE;
    USING_NS_UTILS;

	{
		class TestA : public Singleton<TestA> {
		public:
			void show() {
				log_info("Test a.");
			}

		friend Singleton<TestA>;
		private:
			TestA() {}
		};

		// TestA a(*(TestA::getInstance()));
		// TestA b = std::move(*(TestA::getInstance()));
		TestA::getInstance()->show();
	}

	{
		class TestB : public SingletonPtr<TestB> {
		public:
			void show() {
				log_info("Test b.");
			}

		friend SingletonPtr<TestB>;
		private:
			TestB() {}
		};
		// TestB a;
		TestB::getInstancePtr()->show();
	}
}


#endif

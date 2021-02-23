#ifndef TEST_C11_KEY_WORDS_HPP 
#define TEST_C11_KEY_WORDS_HPP 

#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"

// c11 关键字测试
void TestC11KeyWords() {
	FUNC_TRACE;
   	USING_NS_UTILS;
	
	{
		log_info("~~~Test key words final~~~");
		
		struct BaseNode {
			// virtual void foo();	
			virtual void foo();
		};

		struct NodeA : public BaseNode {
			virtual void foo() final; // 最终覆盖函数
		};

		struct NodeAa final : public NodeA {
			//void foo();            // 上层foo已经被 final标识, 无法再次继承实现 
		};

		// 上层NodeAa 已经被final标识, 无法再次被继承实现
		//struct NodeAb : public NodeAa {    
		//};
	}

	{
		log_info("~~~Test key words override~~~");
		struct BaseNode {
			// virtual void foo();
			virtual void foo(); 
			void car();
		};

		struct NodeA : public BaseNode {
			void foo() override;    	    // override 相当于检测基类是否为虚函数 	
			//void foo() const override;    // 带有const 限定符与基类不同 	
			//void bar() override;          // 基类不存在bar函数 不可重写编译失败
			//void car() override;            // 基类car为非虚函数 不可重写编译失败  
		};

		struct NodeAa : public NodeA {
			void foo() override; 
		};
	}
}


#endif

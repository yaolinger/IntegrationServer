#ifndef TEST_SYNTAX_TEST_TEMPLATE_HPP
#define TEST_SYNTAX_TEST_TEMPLATE_HPP

#include <vector>
#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"

// 模板参数推导 在编译期进行模板推导

//template<typename A, typename B>
//void funcTest(A a, B b); 
//
//template<typename B>
//void funcTest(uint32 arg, B b) {
//	log_info("偏特化A[%u]", arg);
//}
//
//template<typename A>
//void funcTest(A a, std::string b) {
//	log_info("偏特化B[%s]", b.c_str());
//}
//
//template<typename A, typename B = uint64>
//void funcTest(A a, B b) {
//	log_info("默认模板参数测试 b[%lu]", b);
//}
//
//template<typename A, typename C = std::vector<A>>
//void funcTestB(A a, C c) {
//	log_info("模板模板参数");
//}

//template<typename A, typename std::vector<A>>
//void funcTestC(A a, std::vector<A> aVec) {
//	aVec.push_back(a);
//	log_info("特化+容器测试 vec.size[%lu]", aVec.size());
//}

//template<typename A>
//class Contain {
//}
//
//template<typename A, typename<typename A> class B>>
//void funcTestD(A a, B b) {
//	b.insert(a);
//	for (auto& value : b) {
//		log_warn("容器模板循环");
//	}
//}

// 注意模板产生一样函数冲突
//template<>
//void funcTest(uint32 arg, std::string b) {
//	log_info("全特化A[%u] B[%s]", b.c_str());
//}
//

void templateTest() {
	FUNC_TRACE;
    USING_NS_UTILS;
	
//	{
//
//		funcTest((int32)1, (int32)2);
//	}
//	{
//
//		funcTest((uint32)7, (char)'a');
//	}
//
//	{
//
//		funcTest((char)'z', std::string("开心"));
//	}
//	{
//
//		funcTest((char)'w', (uint64)3);
//	} 
//
//	{
//		uint32 a =1;
//		std::vector<uint32> vVec;
//		funcTestB(a, vVec);
//		//funcTestC(a, vVec);
//	}
//
//	{
//		uint32 a =1;
//		std::set<uint32> zSet;
//		funcTestD(a, zSet);
//	}
}

#endif

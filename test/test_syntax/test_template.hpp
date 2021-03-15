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

//template<typename A, template<typename U> typename B>
//void funcTestD(A a, B b) {
//}

//template<typename A, template<typename U> typename B>
//class TestTemplateClass {
//public:
//
//private:
//	B<A> mData;
//};
//
//template <typename T>
//class TestParam {
//};
//
//template <typename T>
//using Myset = std::set<T, std::less<T>, std::allocator<T>>;
// 注意模板产生一样函数冲突
//template<>
//void funcTest(uint32 arg, std::string b) {
//	log_info("全特化A[%u] B[%s]", b.c_str());
//}

//template <typename T, uint32 N, typename C = std::vector<T>>
//void funcTest(C t) {
//	log_info("Vector size[%lu], N[%u]", t.size(), N);
//}

void TestTemplate() {
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
//		funcTestD<uint32, std::set<uint32>>(a, zSet);
//	}

//	{
//		std::vector<uint32> vec;		
//		funcTest<uint32, 10>(vec);
//	}
//
//	{
//		TestTemplateClass<uint32, TestParam> temaplateA;
//		TestTemplateClass<uint32, Myset> temaplateB;
//	}
}

#endif

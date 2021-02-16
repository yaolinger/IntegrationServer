#ifndef TEST_SYNTAX_TEST_HEAP_HPP
#define TEST_SYNTAX_TEST_HEAP_HPP

#include <algorithm>
#include <string>
#include <vector>
#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"

// vector 输出
void PrintVec(const std::string& format, const std::vector<uint32>& dataVec) {
	std::string data;
	for (const auto& value : dataVec) {
		if (data.size() != 0) {
			data += " ";
		}
		data += std::to_string(value);
	}
	log_info("%s data vec[%s]", format.c_str(), data.c_str());
}

// 小根堆
class MinCompare {
public:
	bool operator() (uint32 a, uint32 b) {
		return a > b;
	}
};

// 大根堆
class MaxCompare {
public:
	bool operator() (uint32 a, uint32 b) {
		return a < b;
	}
};

// 自定义测试堆
template<typename Compare>
class XHeap {
public:
	XHeap(const std::vector<uint32>& initVec) {
		PrintVec("Init vec", initVec);		
		mDataVec = initVec;
		// 初次构造堆
		std::make_heap(mDataVec.begin(), mDataVec.end(), Compare());
		PrintVec("Make heap", mDataVec);		
	}

public:
	// 添加堆数据
	void pushHeap(uint32 value) {
		mDataVec.push_back(value);
		PrintVec("Add vector", mDataVec);		
		// 调整堆数据
		std::push_heap(mDataVec.begin(), mDataVec.end(), Compare());
		PrintVec("push_heap", mDataVec);		
	}

	// 删除堆顶元素
	void popHeap() {
		// 调整堆数据
		std::pop_heap(mDataVec.begin(), mDataVec.end(), Compare());
		PrintVec("pop_heap", mDataVec);		
		mDataVec.pop_back();
		PrintVec("pop vector", mDataVec);		
	}

	// 进行堆排序
	void sortHeap() {
		std::sort_heap(mDataVec.begin(), mDataVec.end(), Compare());
		PrintVec("sort heap", mDataVec);		
	}

private:
	std::vector<uint32> mDataVec;    // 数组数据
};

void TestHeap() {
    FUNC_TRACE;
    USING_NS_UTILS;
	std::vector<uint32> testVec;
	testVec.push_back(67);
	testVec.push_back(32);
	testVec.push_back(33);
	testVec.push_back(1);
	testVec.push_back(97);
	testVec.push_back(22);
	testVec.push_back(7);

	XHeap<MinCompare> minHeap(testVec);
	minHeap.pushHeap(9);
	minHeap.popHeap();
	minHeap.sortHeap();
}

#endif

#ifndef STRESS_TEST_STRESS_TEST_MANAGER_HPP
#define STRESS_TEST_STRESS_TEST_MANAGER_HPP

#include <mutex>
#include <string>
#include <vector>

#include "utils/log.hpp"
#include "utils/macros.hpp"

// 压测数据
struct StressData {
	StressData(uint64 time1, uint64 time2) : begin(time1), end(time2) {
	}
	uint64 begin = 0;
	uint64 end = 0;
};

// 压力测试数据汇总
class StressTestManager {
public:
	StressTestManager(std::string& name) : mName(std::move(name)) {};
	~StressTestManager() {}

public:
	// 数据重置
	void reset();
	// 插入数据(ms)
	void insertDataMS(uint64 begin, uint64 end);
	// 展示数据
	void display();

private:
	std::string mName;                       // 数据名称
	std::mutex mDataMutex;                   // 数据锁
	std::vector<StressData> mStressDataVec;  // 压测数据
};

#endif

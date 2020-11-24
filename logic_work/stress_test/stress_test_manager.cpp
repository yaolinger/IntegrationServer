#include "stress_test_manager.hpp"

#include "utils/calc_tool.hpp"

USING_NS_UTILS;

void StressTestManager::reset() {
	std::lock_guard<std::mutex> lk(mDataMutex);
	mStressDataVec.clear();
}

void StressTestManager::insertDataMS(uint64 begin, uint64 end) {
	StressData data(begin, end);
	std::lock_guard<std::mutex> lk(mDataMutex);
	mStressDataVec.push_back(data);
}


void StressTestManager::display() {
	uint64 dataCount = 0;
	uint64 allResponseTimes = 0;
	uint64 averageResponseTimes = 0;
	uint64 minResponseTimes = MAX_UINT64_VALUE;
	uint64 maxResponseTimes = 0; 
	
	{
		std::lock_guard<std::mutex> lk(mDataMutex);
		dataCount = mStressDataVec.size();
		for (auto& value : mStressDataVec) {
			uint64 dValue = safeSub(value.end, value.begin); 
			allResponseTimes += dValue;
			if (dValue < minResponseTimes) {
				minResponseTimes = dValue;
			}

			if (dValue > maxResponseTimes) {
				maxResponseTimes = dValue;
			}
		}
	}

	log_warn("************************[%s]:压测数据统计********************************", mName.c_str());
	if (dataCount > 0) {
		averageResponseTimes = allResponseTimes / dataCount; 
	}
	log_info("Data count[%lu] Times: all[%lu] average[%lu] min[%lu] max[%lu]", dataCount, allResponseTimes, averageResponseTimes, minResponseTimes, maxResponseTimes);
	log_warn("************************[%s]:压测数据统计********************************", mName.c_str());
}

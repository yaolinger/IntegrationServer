#ifndef UTILS_SIGNAL_CONTROL_HPP
#define UTILS_SIGNAL_CONTROL_HPP

#include <set>
#include "scheduler_base.hpp"

NS_UTILS_BEGIN

// 完全采用静态类(单一用途)

// 信号控制
class SignalControl {
public:
	// 初始化
	static bool init(SchedulerPtr pScheduler);
	// 释放
	static void reset();
	// 添加忽略信号
	static bool addIgnoreSignal(int32 signalNum);
	// 添加信号
	static void addSignal(int32 signalNum);
	// 信号处理接口
	static void asyncWait(std::function<void(int32)> signalFunc);
	// 捕捉到信号
	static void catchSignal(int32 signalNum);

private:
	static std::set<int32> sSignalSet;             // 信号集合
	static SchedulerPtr sScheduler;                // 调度器
	static std::function<void(int32)> sFuncWarp;   // 函数包装器
};

NS_UTILS_END

#endif

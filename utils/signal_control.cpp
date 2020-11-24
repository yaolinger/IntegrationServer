#include "signal_control.hpp"

#include <signal.h>
#include "scheduler_unit.hpp"

NS_UTILS_BEGIN

void SignalHandler(int32 signalNum) {
	SignalControl::catchSignal(signalNum);
}

std::set<int32> SignalControl::sSignalSet;             // 信号集合
SchedulerPtr SignalControl::sScheduler;                // 调度器
std::function<void(int32)> SignalControl::sFuncWarp;   // 函数包装器

bool SignalControl::init(SchedulerPtr pScheduler) {
	if (NULL == pScheduler) {
		return false;
	}
	sScheduler = pScheduler;
	return true;
}

void SignalControl::reset() {
	if (sScheduler) {
		sScheduler.reset();
	}
}

bool SignalControl::addIgnoreSignal(int32 signalNum) {
    // 忽略信号signalNum
	struct sigaction sigIgnore;
	sigIgnore.sa_handler = SIG_IGN;
	return sigaction(signalNum, &sigIgnore, NULL);
}

void SignalControl::addSignal(int32 signalNum) {
	sSignalSet.insert(signalNum);
}

void SignalControl::asyncWait(std::function<void(int32)> signalFunc) {
	if (sSignalSet.empty()) {
		return;
	}
	sFuncWarp = [signalFunc](int32 signalNum) {
		auto waitFunc = std::bind(signalFunc, signalNum);
		UnitPtr pUnit = std::make_shared<SchdeulerUnit>(waitFunc);
		sScheduler->post(pUnit);
	};
	struct sigaction sigHandler;
	sigHandler.sa_handler = SignalHandler;
	for (auto& value : sSignalSet) {
		sigaction(value, &sigHandler, NULL);
	}
}

void SignalControl::catchSignal(int32 signalNum) {
	sFuncWarp(signalNum);
}

NS_UTILS_END

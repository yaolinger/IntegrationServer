#include "reactor_fake.hpp"

#include <chrono>
#include <thread>

#include "log.hpp"
#include "rand.hpp"
#include "scheduler_unit.hpp"

NS_UTILS_BEGIN

void ReactorFake::reactorWait(std::list<UnitPtr>& taskList, int32 timeout) {
    log_debug("Thread[%lu] run reactor wait.", std::this_thread::get_id());

    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));

    // 随机数据包
    uint32 count = Rand::randBetween((uint32)1, (uint32)5);
    for (uint32 i = 0; i < count; i++) {
        UnitPtr pUnit = std::make_shared<SchdeulerUnit>([](){
                    log_debug("随机任务[%lu]",  std::this_thread::get_id());
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                });
        taskList.push_back(pUnit);
    }
}

NS_UTILS_END

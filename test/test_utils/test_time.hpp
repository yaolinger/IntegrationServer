#ifndef TEST_UTILS_TEST_TIME_HPP
#define TEST_UTILS_TEST_TIME_HPP

#include "utils/time_helper.hpp"
#include "utils/log.hpp"

void TestTimer() {
   USING_NS_UTILS
   StopWatch sw;

   std::this_thread::sleep_for(second(1));
   log_info("Duration %d (us)", sw.durationUs());
   log_info("Duration %d (ms)", sw.durationMs());
   log_info("Duration %d (s)", sw.durationS());

   int32 now = TimeHelper::getCurrentSecond();
   std::string nowStr = TimeHelper::getTimeStr(now);
   log_warn("Date: %s, %d (s).", nowStr.c_str(), now);
}

#endif


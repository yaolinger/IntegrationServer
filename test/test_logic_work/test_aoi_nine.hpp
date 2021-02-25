#ifndef TEST_LOGIC_WORK_TEST_AOI_NINE_HPP
#define TEST_LOGIC_WORK_TEST_AOI_NINE_HPP

#include "logic_work/aoi_nine/nine_map.hpp"
#include "utils/log.hpp"
#include "utils/macros.hpp"

void TestAoiNine() {
	FUNC_TRACE;
	USING_NS_UTILS;
	NineMap nine;
	nine.init(270, 90);
	nine.displayNineMap();
}

#endif

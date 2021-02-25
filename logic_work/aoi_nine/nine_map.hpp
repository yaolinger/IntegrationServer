#ifndef AOI_NINE_NINE_MAP_HPP
#define AOI_NINE_NINE_MAP_HPP

#include <array>
#include "nine_common.hpp"

typedef std::vector<uint32> nineVec;  			// 单一九宫格
typedef std::map<uint32, nineVec> nineMap;      // 九宫格map

// 九宫格地图
class NineMap {
public:
	// 初始化
	bool init(uint32 maxX, uint32 maxY);
	// 地图展示
	void displayNineMap();

private:
	uint32 mMaxX = 0;  			// 最大x轴
	uint32 mMaxY = 0;  			// 最大y轴

	uint32 mMaxSectorX = 0;     // 最大屏宽
	uint32 mMaxSectorY = 0;     // 最大屏高
	uint32 mMaxMapSize = 0;     // 最大屏数量
	
	nineMap mNineMap;           // 九宫格map
	std::array<nineMap, NINE_DIR_TYPE_CENTER> mDirectMap;  			// 正向变化5屏或者3屏
	std::array<nineMap, NINE_DIR_TYPE_CENTER> mReverseDirectMap;    // 反向变化5屏或者3屏
};

#endif

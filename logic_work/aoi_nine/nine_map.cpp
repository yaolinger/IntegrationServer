#include "nine_map.hpp"

#include <string>
#include <sstream>
#include "utils/log.hpp"

bool NineMap::init(uint32 maxX, uint32 maxY) {
	mMaxX = maxX;
	mMaxY = maxY;

	log_info("地图max x轴长度[%u] 地图max y轴长度[%u].", mMaxX, mMaxY);

	// 计算格子信息
	mMaxSectorX = (mMaxX + SCREEN_WIDTH -1) / SCREEN_WIDTH;
	mMaxSectorY = (mMaxY + SCREEN_HEIGHT - 1) / SCREEN_HEIGHT;	
	mMaxMapSize = mMaxSectorX * mMaxSectorY;

	log_info("x轴屏数量[%u] y轴屏数量[%u] 最大屏数量[%u].", mMaxSectorX, mMaxSectorY, mMaxMapSize);

	for(uint32 y = 0; y < mMaxSectorY; y++) {
		for (uint32 x = 0; x < mMaxSectorX; x++) {
			uint32 key = y * mMaxSectorX + x;
			// 计算九宫格关系
			{
				nineVec nineCell;
				for (uint32 i = 0; i < NINE_ARRAY_FIRST_SIZE; i++) {
					int32 cellX = x + nineIndexArray[i][0];
					int32 cellY = y + nineIndexArray[i][1];
					// 过略失效边界, 边界格子不满足九宫格, 存在无效格子	
					if (cellX >= 0 && cellX < (int32)mMaxSectorX && cellY >= 0 && cellY < (int32)mMaxSectorY) {
						nineCell.push_back(cellY * mMaxSectorX + cellX);
					}
				}
				mNineMap.insert(std::make_pair(key, nineCell));
			}
			// 计算正向变化5格或者3格
			// 除去中心点8个方向
			for (uint32 dir = 0; dir < NINE_DIR_TYPE_CENTER; dir++) {
				uint32 start, end;
				nineVec nineCell;
				if (1 == dir % 2) {
					//斜方向
					start = 6;
					end = 10;
				} else {
					//正方向
					start = 7;
					end = 9;
				}

				for (uint32 i = start; i <= end; i++) {
					uint32 iDir = (i + dir) % 8;
					int32 cellX = x + nineIndexArray[iDir][0];
					int32 cellY = y + nineIndexArray[iDir][1];
					// 过略失效边界, 边界格子不满足九宫格, 存在无效格子	
					if (cellX >= 0 && cellX < (int32)mMaxSectorX && cellY >= 0 && cellY < (int32)mMaxSectorY) {
						nineCell.push_back(cellY * mMaxSectorX + cellX);
					}
				}
				mDirectMap[dir].insert(std::make_pair(key, nineCell));
			}

			// 计算反向变化的5格或者3格
			// 去中心点8个方向
			for (uint32 dir = 0; dir < NINE_DIR_TYPE_CENTER; dir++) {
				uint32 start, end;
				nineVec nineCell;
				if (1 == dir % 2) {
					//斜方向
					start = 2;
					end = 6;
				} else {
					//正方向
					start = 3;
					end = 5;
				}

				for (uint32 i = start; i <= end; i++) {
					uint32 iDir = (i + dir) % 8;
					int32 cellX = x + nineIndexArray[iDir][0];
					int32 cellY = y + nineIndexArray[iDir][1];
					// 过略失效边界, 边界格子不满足九宫格, 存在无效格子	
					if (cellX >= 0 && cellX < (int32)mMaxSectorX && cellY >= 0 && cellY < (int32)mMaxSectorY) {
						nineCell.push_back(cellY * mMaxSectorX + cellX);
					}
				}
				mReverseDirectMap[dir].insert(std::make_pair(key, nineCell));
			}
		}
	}

	return true;
}

void NineMap::displayNineMap() {
	auto logVecFunc = [](const std::vector<uint32>& cellVec) -> std::string {
		std::ostringstream sstr;
		bool first = true;
		for (auto& value : cellVec) {
			if (first) {
				first = false;
			} else {
				sstr << ",";
			}
			sstr << value;
		}
		return sstr.str();
	};

	for (auto& kv : mNineMap) {
		log_info("Center cell[%u] base nine[%s]", kv.first, logVecFunc(kv.second).c_str());
		for (uint32 i = 0; i < NINE_DIR_TYPE_CENTER; i++) {
			log_info("Center cell[%u] dir[%u] direct nine[%s]", kv.first, i, logVecFunc(mDirectMap[i][kv.first]).c_str());
		}
		for (uint32 i = 0; i < NINE_DIR_TYPE_CENTER; i++) {
			log_info("Center cell[%u] dir[%u] direct nine[%s]", kv.first, i, logVecFunc(mReverseDirectMap[i][kv.first]).c_str());
		}
	}
}

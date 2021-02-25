#ifndef AOI_NINE_NINE_COMMON_HPP
#define AOI_NINE_NINE_COMMON_HPP

#include <map>
#include <memory>
#include <vector>
#include "utils/macros.hpp"

#define SCREEN_WIDTH 30    		 // 屏宽
#define SCREEN_HEIGHT 30   		 // 屏高
#define NINE_ARRAY_FIRST_SIZE 9  // 九宫格数组第一维度长度
#define NINE_ARRAY_SECOND_SIZE 2 // 九宫格数组第二维度长度 


/* 九宫格图样
 *
 *5(-1, 1)   4( 0, 1)   3( 1, 1)
 * 
 *6(-1, 0)   8( 0, 0)   2( 1, 0)
 *     
 *7(-1,-1)   0( 0,-1)   1( 1,-1)
 * 
 * */

// 九宫格索引(nineIndexArray[0]~nineIndexArray[7]为一圈方向格子, nineIndexArray[8]为中心格子)
static int32 nineIndexArray[NINE_ARRAY_FIRST_SIZE][NINE_ARRAY_SECOND_SIZE] = {{0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, 0}};

// 九宫方向
enum NINE_DIR_TYPE {
	NINE_DIR_TYPE_UP = 0,       	// 上
	NINE_DIR_TYPE_RIGHT_UP = 1,  	// 右上
	NINE_DIR_TYPE_RIGHT = 2,    	// 右
	NINE_DIR_TYPE_RIGHT_DOWN = 3,	// 右下
	NINE_DIR_TYPE_DOWN = 4,         // 下
	NINE_DIR_TYPE_LEFT_DOWN = 5,    // 左下
	NINE_DIR_TYPE_LEFT = 6,         // 左
	NINE_DIR_TYPE_LEFT_UP = 7,      // 左上

	NINE_DIR_TYPE_CENTER = 8,       // 中心点
	NINE_DIR_TYPE_NOT_CROSS = 9,    // 不相交
};


// 坐标
struct Pos {
	Pos() {
	}
	Pos(int32 xPos, int32 yPos, int32 dirPos) : mX(xPos), mY(yPos), mDir(dirPos) {
	}

	int32 mX = 0;          // x轴
	int32 mY = 0;		   // y轴
	int32 mDir = 0;		   // 方向
};

#endif

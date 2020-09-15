#ifndef BATTLE_BATTLE_COMMON_HPP
#define BATTLE_BATTLE_COMMON_HPP

#include <map>
#include <memory>
#include <vector>
#include "utils/macros.hpp"

// 战斗阵营
enum BATTLE_CAMP {
    BATTLE_CAMP_1 = 1,      // 阵营1
    BATTLE_CAMP_2 = 2,      // 阵营2
};

// 战斗阶段
enum BATTLE_STEP {
    BATTLE_STEP_INIT = 1,     // 初始化阶段
    BATTLE_STEP_LOADING = 2,  // 加载数据阶段
    BATTLE_STEP_ROUND = 3,    // 回合战斗阶段
    BATTLE_STEP_END = 4,      // 战斗结束
};

// 战斗状态
enum BATTLE_STATE {
    BATTLE_STATE_INIT = 1,                  // 初始状态
    BATTLE_STATE_WAITING_OPERATE = 2,       // 等他玩家操作状态
    BATTLE_STATE_FIGHTING = 3,              // 计算战斗状态
    BATTLE_STATE_PLAYING = 4,               // 播放战斗状态
};

// 阵营单元状态
enum CAMP_UNIT_STATE {
    CAMP_UNIT_STATE_INIT = 1,        // 初始化状态
    CAMP_UNIT_STATE_LOADING = 2,     // 加载数据状态
    CAMP_UNIT_STATE_LOADING_END = 3, // 加载数据完成
    CAMP_UNIT_STATE_OPERATE = 4,     // 玩家操作ing
    CAMP_UNIT_STATE_OPERATE_END = 5, // 玩家操作结束
    CAMP_UNIT_STATE_PLAY = 6,        // 播放状态
    CAMP_UNIT_STATE_PLAY_END = 7,    // 播放状态结束
};

// 战斗单元类型
enum BATTLE_UNIT_TYPE {
    BATTLE_UNIT_TYPE_ROBOT = 1,        // 机器人
    BATTLE_UNIT_TYPE_USER = 2,         // 玩家
};

// 战斗单元数据
struct BattleUnitData {
    uint64 uid;                           // 唯一id(英雄, 怪物)
    std::map<uint32, uint32> attrMap;     // 属性数据
};

// 战斗阵营数据
struct BattleCamp {
    uint64 uid;                               // 唯一id(玩家, 机器人)
    uint32 camp;                              // 阵营
    uint32 type;                              // 类型
    std::map<uint64, BattleUnitData> unitMap; // 阵营成员
};

// 战斗参数
struct BattleParam {
    uint32 type;                          // 战斗类型
};

// 战斗初始数据
struct BattleInitData {
    BattleParam param;                    // 战斗参数
    std::vector<BattleCamp> campVec;      // 阵营数据
};

// 战斗属性
enum BATTLE_ATTR {
    BATTLE_ATTR_HP = 1,                   // 血量
    BATTLE_ATTR_ATK = 2,                  // 攻击力
    BATTLE_ATTR_DEF = 3,                  // 防御力
    BATTLE_ATTR_SPEED = 4,                // 速度
};

// 战斗操作
struct CombatOperate {
    uint32 skillId = 0;                       // 技能id
    std::vector<uint64> targetVec;            // 目标
};

// 战斗结果
enum COMBAT_RESULT {
    COMBAT_RESULT_WIN = 1,                    // 胜利
    COMBAT_RESULT_TIE = 2,                    // 平局
    COMBAT_RESULT_DEFEAT = 3,                 // 失败
    COMBAT_RESULT_TIMEOUT = 4,                // 超时
};

// 战斗结果
struct CombatResult {
    uint32 ret;
};

// 获取进度条
#define GET_SPEED_BAR(MAX_SPEED) MAX_SPEED * 7
// 跑条次数无效值
#define RUN_SPEED_BAR_INVALID 99999
// 最大回合总数
#define MAX_ROUND  99
// 测试技能id
#define TEST_SKILL_ID 99999

#endif

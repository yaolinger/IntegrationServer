#ifndef BATTLE_BATTLE_ATTR_HPP
#define BATTLE_BATTLE_ATTR_HPP

#include<map>
#include<set>
#include<vector>
#include "utils/macros.hpp"

// 战斗属性
enum BATTLE_ATTR {
    BATTLE_ATTR_HP = 1,                   // 血量
    BATTLE_ATTR_ATK = 2,                  // 攻击力
    BATTLE_ATTR_DEF = 3,                  // 防御力
    BATTLE_ATTR_SPEED = 4,                // 速度
};

// 战斗计算最小粒度(元素), skill 最多含有3个eSet，eSet最多含有3个元素
/*                       element1
 *            _eSet1 ==> element2
 *           |           element3
 *           |              .
 * skill  ==>|_eSet2        .
 *           |              .
 *           |              .
 *           |_eSet3        .
 *                          .
 */

// 元素
struct BattleElement {
    uint32 index;                        // 元素索引
    std::vector<uint32> paramVec;        // 元素参数(不同的元素需要的参数不同)
};

// 元素集合
struct BattleESet {
    uint32 index;                        // 集合索引
    bool isBuff;                         // 是否为buff
    std::set<uint32> elementSet;         // 元素集合
};

// 技能
struct BattleSkill {
    uint32 index;                        // 技能索引
    std::set<uint32> eSetSet;            // 元素集合的集合
};

// 元素枚举
enum BATTLE_ELEMENT {
    BATTLE_ELEMENT_NORMAL_ATK = 1,        // 普通攻击
};

// 元素集合枚举
enum BATTLE_ESET {
    BATTLE_ESET_NORMAL_ATK = 1,           // 普通攻击
};

// 技能枚举
enum BATTLE_SKILL {
    BATTLE_SKILL_NORMAL_ATK = 1,          // 普通攻击
};

#endif

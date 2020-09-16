#ifndef BATTLE_COMBAT_CALC_HPP
#define BATTLE_COMBAT_CALC_HPP

#include "combat.hpp"
#include "battle_attr.hpp"

// 战斗元素计算
// pCurUnit:攻击者 pTarget:目标 pCombat:所属战斗
typedef std::function<void(BattleUnitPtr pCurUnit, BattleUnitPtr pTarget, CombatPtr pCombat)> battleElementFunc;

// 战斗计算
class CombatCalc {
public:
    // 初始化
    static void init();
    // 初始化配置
    static void initConifg();
    // 执行操作(技能)
    static void doOperate(BattleUnitPtr pCurUnit, CombatPtr pCombat);
// 元素计算
private:
    // BATTLE_ELEMENT_NORMAL_ATK
    static void elementNormalAtk(BattleUnitPtr pCurUnit, BattleUnitPtr pTargetUnit, CombatPtr pCombat);
private:
    static std::map<uint32, battleElementFunc> s_elementFuncMap;    // 元素处理接口

// 配置暂时使用静态写死
private:
    static std::map<uint32, BattleSkill> s_skillConfigMap;     // 技能配置
    static std::map<uint32, BattleESet>  s_eSetConfigMap;      // 元素集合配置
    static std::map<uint32, BattleElement> s_elementConfigMap; // 元素配置
};

#endif

#ifndef BATTLE_COMBAT_CALC_HPP
#define BATTLE_COMBAT_CALC_HPP

#include "combat.hpp"

// 战斗计算
class CombatCalc {
public:
    // 执行操作
    static void doOperate(BattleUnitPtr pCurUnit, CombatPtr pCombat);

private:
    // 测试技能
    static void testSkill(BattleUnitPtr pCurUnit, BattleUnitPtr pTargetUnit);

};

#endif

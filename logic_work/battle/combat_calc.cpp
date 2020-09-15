#include "combat_calc.hpp"

#include "utils/calc_tool.hpp"
#include "utils/log.hpp"

USING_NS_UTILS;

void CombatCalc::doOperate(BattleUnitPtr pCurUnit, CombatPtr pCombat) {
    if (NULL == pCurUnit) {
        log_error("NULL == pCurUnit");
        return;
    }
    if (NULL == pCombat) {
        log_error("NULL == pCombat");
        return;
    }

    // 获取当前操作
    const CombatOperate& op = pCurUnit->getCurOperate();
    if (op.skillId == TEST_SKILL_ID) {
        for(auto& value : op.targetVec) {
            BattleUnitPtr pTargetUnit = pCombat->getBattleUnit(value);
            testSkill(pCurUnit, pTargetUnit);
        }
    } else {
        log_warn("skillId[%u] 暂时只支持测试技能", op.skillId);
    }
}


void CombatCalc::testSkill(BattleUnitPtr pCurUnit, BattleUnitPtr pTargetUnit) {
    uint32 attackAtk = pCurUnit->getAtk();
    uint32 targetDef = pTargetUnit->getDef();
    uint32 targetHp = pTargetUnit->getHp();

    uint32 costHp = safeSub(attackAtk, targetDef);
    uint32 leftHp = safeSub(targetHp, costHp);
    log_info("Unit[%u:%lu] attack uint[%u:%lu] preHp[%u] costHp[%u] leftHp[%u]", pCurUnit->getCamp(), pCurUnit->getUid(), pTargetUnit->getCamp(), pTargetUnit->getUid(), targetHp, costHp, leftHp);
    pTargetUnit->setHp(leftHp);
}

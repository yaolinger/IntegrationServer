#include "combat_calc.hpp"

#include "utils/calc_tool.hpp"
#include "utils/log.hpp"

USING_NS_UTILS;

std::map<uint32, battleElementFunc> CombatCalc::s_elementFuncMap;
std::map<uint32, BattleSkill> CombatCalc::s_skillConfigMap;
std::map<uint32, BattleESet>  CombatCalc::s_eSetConfigMap;
std::map<uint32, BattleElement> CombatCalc::s_elementConfigMap;

void CombatCalc::init() {
    initConifg();
    s_elementFuncMap.insert(std::make_pair(BATTLE_ELEMENT_NORMAL_ATK, elementNormalAtk));
}

void CombatCalc::initConifg() {
    // 元素配置
    {
        // 元素:普通攻击 param:1.攻击力百分比 公式 atk * (param*1.0f/100)
        BattleElement element;
        element.index = BATTLE_ELEMENT_NORMAL_ATK;
        element.paramVec.push_back(150);
        s_elementConfigMap[element.index] = element;
    }
    // 元素集合配置
    {
        // 元素集合:普通攻击
        BattleESet eSet;
        eSet.index = BATTLE_ESET_NORMAL_ATK;
        eSet.isBuff = false;
        eSet.elementSet.insert(BATTLE_ELEMENT_NORMAL_ATK);
        s_eSetConfigMap[BATTLE_ESET_NORMAL_ATK] = eSet;
    }
    // 技能配置
    {
        // 技能:普通攻击
        BattleSkill skill;
        skill.index = BATTLE_SKILL_NORMAL_ATK;
        skill.eSetSet.insert(BATTLE_ESET_NORMAL_ATK);
        s_skillConfigMap[BATTLE_SKILL_NORMAL_ATK] = skill;
    }

    log_warn("Init skill element success~~~~");
}

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
    if (s_skillConfigMap.find(op.skillId) == s_skillConfigMap.end()) {
        log_error("Can not find skill[%u] config.", op.skillId);
        return;
    }

    // 执行元素集合
    auto doESet = [&](const BattleESet& eSet) {
        if (eSet.isBuff) {
            //TODO:: buff操作暂时未实现
            log_warn("ESet[%u] is buff 暂时未实现", eSet.index);
        }
        for (const uint32 value : eSet.elementSet) {
            if (s_elementFuncMap.find(value) == s_elementFuncMap.end()) {
                log_error("Can not find element[%u] func.", value);
                continue;
            }
            for(auto& uid : op.targetVec) {
                BattleUnitPtr pTargetUnit = pCombat->getBattleUnit(uid);
                if (NULL == pTargetUnit) {
                    log_error("NULL == pTargetUnit[%lu]", uid);
                    continue;
                }
                s_elementFuncMap[value](pCurUnit, pTargetUnit, pCombat);
            }
        }
    };
    // 执行技能
    const BattleSkill& skill = s_skillConfigMap[op.skillId];
    for (const uint32 value : skill.eSetSet) {
        if (s_eSetConfigMap.find(value) == s_eSetConfigMap.end()) {
            log_error("Skill[%u] eSet[%u] config can not find.", op.skillId, value);
            continue;
        }
        const BattleESet& eSet = s_eSetConfigMap[value];
        doESet(eSet);
    }
}

void CombatCalc::elementNormalAtk(BattleUnitPtr pCurUnit, BattleUnitPtr pTargetUnit, CombatPtr pCombat) {
    if (s_elementConfigMap.find(BATTLE_ELEMENT_NORMAL_ATK) == s_elementConfigMap.end()) {
        log_error("Can not find element[%u]", BATTLE_ELEMENT_NORMAL_ATK);
        return;
    }

    // 获取元素
    const BattleElement& element = s_elementConfigMap[BATTLE_ELEMENT_NORMAL_ATK];
    uint32 attackAtk = pCurUnit->getAtk();
    uint32 targetDef = pTargetUnit->getDef();
    uint32 targetHp = pTargetUnit->getHp();

    // 计算伤害
    uint32 costHp = safeSub(uint32(attackAtk * (element.paramVec[0] * 1.0f / 100)), targetDef);
    uint32 leftHp = safeSub(targetHp, costHp);
    log_info("Unit[%u:%lu] attack uint[%u:%lu] preHp[%u] costHp[%u] leftHp[%u]", pCurUnit->getCamp(), pCurUnit->getUid(), pTargetUnit->getCamp(), pTargetUnit->getUid(), targetHp, costHp, leftHp);
    pTargetUnit->setHp(leftHp);

}

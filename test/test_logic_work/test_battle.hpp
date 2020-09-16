#ifndef TEST_LOGIC_WORK_TEST_BATTLE_HPP
#define TEST_LOGIC_WORK_TEST_BATTLE_HPP

#include "logic_work/battle/combat.hpp"
#include "logic_work/battle/combat_calc.hpp"
#include "utils/log.hpp"
#include "utils/log_helper.hpp"
#include "utils/macros.hpp"
#include "utils/rand.hpp"

static uint64 s_campUid = 1;
static uint64 getCampUid() { return s_campUid++; }

static uint64 s_unitUid = 100;
static uint64 getUnitUid() { return s_unitUid++; }

BattleUnitData RandUnit(uint64 uid) {
    BattleUnitData data;
    data.uid = uid;
    data.attrMap[BATTLE_ATTR_HP] = UTILS::Rand::randBetween(uint32(100), uint32(200));
    data.attrMap[BATTLE_ATTR_ATK] = UTILS::Rand::randBetween(uint32(20), uint32(50));
    data.attrMap[BATTLE_ATTR_DEF] = UTILS::Rand::randBetween(uint32(5), uint32(15));
    data.attrMap[BATTLE_ATTR_SPEED] = UTILS::Rand::randBetween(uint32(1), uint32(5));
    return data;
}

void TestBattle() {
    FUNC_TRACE;
	USING_NS_UTILS;

    CombatCalc::init();

    CombatPtr pCombat = std::make_shared<Combat>(1);
    BattleInitData battleData;
    {
       BattleCamp camp1;
       camp1.uid = getCampUid();
       camp1.camp = BATTLE_CAMP_1;
       camp1.type = BATTLE_UNIT_TYPE_ROBOT;
       uint32 count = UTILS::Rand::randBetween(uint32(1), uint32(5));
       for (uint32 i = 0; i < count; i++) {
         BattleUnitData data = RandUnit(getUnitUid());
         camp1.unitMap[data.uid] = data;
       }
       battleData.campVec.push_back(camp1);
    }
    {
       BattleCamp camp2;
       camp2.uid = getCampUid();
       camp2.camp = BATTLE_CAMP_2;
       camp2.type = BATTLE_UNIT_TYPE_ROBOT;
       uint32 count = UTILS::Rand::randBetween(uint32(1), uint32(5));
       for (uint32 i = 0; i < count; i++) {
         BattleUnitData data = RandUnit(getUnitUid());
         camp2.unitMap[data.uid] = data;
       }
       battleData.campVec.push_back(camp2);
    }

    bool result = pCombat->init(battleData);
    if (!result) {
        log_error("战斗创建失败");
    }
}

#endif

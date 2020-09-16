#include "battle_unit.hpp"

#include "battle_attr.hpp"

BattleUnit::BattleUnit() {}

BattleUnit::~BattleUnit() {}

bool BattleUnit::init(uint32 camp, const BattleUnitData& data) {
    m_uid = data.uid;
    m_camp = camp;
    for (const auto& kv : data.attrMap) {
        setAttr(kv.first, kv.second);
        // 血量初始化
        if (kv.first == BATTLE_ATTR_HP) {
            m_attr.hp = kv.second;
        }
    }
    return true;
}

void BattleUnit::setAttr(uint32 type, uint32 value) {
    m_baseAttrMap[type] = value;
}

uint32 BattleUnit::getAttr(uint32 type) {
    auto iter = m_baseAttrMap.find(type);
    if (iter == m_baseAttrMap.end()) {
        return 0;
    }
    return iter->second;
}

bool BattleUnit::isCanRunSpeedBar() {
    // TODO::后续增加特殊buff不可跑条(眩晕,冰封。。。)
    return true;
}

void BattleUnit::setCurOperate(uint32 skill, std::vector<uint64>& targetVec) {
    m_curOpearte.skillId = skill;
    m_curOpearte.targetVec.swap(targetVec);
}

void BattleUnit::clearCurOperate() {
    m_curOpearte.skillId = 0;
    m_curOpearte.targetVec.clear();
}

uint32 BattleUnit::getDefaultSkill() {
    // TODO::暂未实现
    return BATTLE_SKILL_NORMAL_ATK;
}

uint32 BattleUnit::getSpeed() {
    return getAttr(BATTLE_ATTR_SPEED);
}

uint32 BattleUnit::getAtk() {
    return getAttr(BATTLE_ATTR_ATK);
}

uint32 BattleUnit::getDef() {
    return getAttr(BATTLE_ATTR_DEF);
}

uint32 BattleUnit::getHp() {
    return m_attr.hp;
}

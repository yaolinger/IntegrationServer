#include "camp_unit.hpp"

CampUnit::CampUnit(uint64 uid, uint32 camp, uint32 type) : m_uid(uid), m_camp(camp), m_type(type) {
    // 设置状态(user玩家loading状态)
    if (BATTLE_UNIT_TYPE_USER == m_type) {
        m_state = CAMP_UNIT_STATE_LOADING;
        m_auto = false;
    } else {
        m_state = CAMP_UNIT_STATE_LOADING_END;
        m_auto = true;
    }
}

CampUnit::~CampUnit() {}

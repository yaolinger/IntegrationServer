#ifndef BATTLE_CAMP_UNIT_HPP
#define BATTLE_CAMP_UNIT_HPP

#include "battle_common.hpp"

// 阵营单元(玩家,机器人)
class CampUnit {
public:
    CampUnit(uint64 uid, uint32 camp, uint32 type);
    ~CampUnit();

public:
    // 获取唯一id
    uint64 getUid() { return m_uid; }
    // 获取阵营
    uint32 getCamp() { return m_camp; }
    // 获取类型
    uint32 getType() { return m_type; }
    // 获取状态
    uint32 getState() { return m_state; }
    // 设置状态
    void setState(uint32 state) { m_state = state; }
    // 获取自动攻击
    bool getAuto() { return m_auto; }
    // 设置自动攻击
    void setAuto(bool autoFight) { m_auto = autoFight; }

private:
    uint64 m_uid;                                    // 唯一id(玩家唯一id，机器人id)
    uint32 m_camp;                                   // 阵营id
    uint32 m_type;                                   // 类型
    uint32 m_state = CAMP_UNIT_STATE_INIT;           // 状态
    bool m_auto;                                     // 自动攻击
};

typedef std::shared_ptr<CampUnit> CampUnitPtr;

#endif

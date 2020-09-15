#ifndef BATTLE_BATTLE_UNIT_HPP
#define BATTLE_BATTLE_UNIT_HPP

#include "battle_common.hpp"

// 战斗变化属性
struct BattleAttr {
    uint32 hp = 0;               // 当前血量
    uint32 curSpeedBar = 0;      // 当前速度条
    uint32 overflowSpeedBar = 0; // 溢出速度条
};

// 战斗单元(英雄,怪物...)
class BattleUnit {
public:
    BattleUnit();
    ~BattleUnit();

    // 战斗单元初始化
    bool init(uint32 camp, const BattleUnitData& data);

// 基础数据
public:
    // 获取唯一id
    uint64 getUid() { return m_uid; }
    // 获取阵营
    uint32 getCamp() { return m_camp; }

// 战斗相关
public:
    // 是否可以跑条(眩晕等不可跑条)
    bool isCanRunSpeedBar();
    // 是否存活
    bool isAlive() { return m_attr.hp > 0; }
    // 设置当前操作
    void setCurOperate(uint32 skill, std::vector<uint64>& targetVec);
    // 获取当前操作
    const CombatOperate& getCurOperate() { return m_curOpearte; }
    // 清空当前操作
    void clearCurOperate();
    // 获取默认技能
    uint32 getDefaultSkill();

    // 获取速度
    uint32 getSpeed();
    // 获取速度条
    uint32 getSpeedBar() { return m_attr.curSpeedBar; }
    // 设置速度条
    void setSpeedBar(uint32 value) { m_attr.curSpeedBar = value; }
    // 获取溢出速度条
    uint32 getOverflowSpeedBiar() { return m_attr.overflowSpeedBar; }
    // 设置溢出速度条
    void setOverflowSpeedBar(uint32 value) { m_attr.overflowSpeedBar = value; }
    // 获取速度条总值
    uint32 getSpeedBarTotal() { return getSpeedBar() + getOverflowSpeedBiar(); }
    // 获取攻击力
    uint32 getAtk();
    // 获取防御力
    uint32 getDef();
    // 获取血量
    uint32 getHp();
    // 设置血量
    void setHp(uint32 value) { m_attr.hp = value; }

private:
    // 设置属性
    void setAttr(uint32 type, uint32 value);
    // 获取属性
    uint32 getAttr(uint32 type);

private:
    uint64 m_uid;                            // 唯一id
    uint32 m_camp;                           // 所属阵营
    std::map<uint32, uint32> m_baseAttrMap;  // 固定属性(不可修改值)
    BattleAttr m_attr;                       // 变化属性(可修改)
    CombatOperate m_curOpearte;              // 当前用户操作
};

typedef std::shared_ptr<BattleUnit> BattleUnitPtr;

#endif

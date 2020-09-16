#ifndef BATTLE_COMBAT_HPP
#define BATTLE_COMBAT_HPP

#include "battle_attr.hpp"
#include "battle_unit.hpp"
#include "camp_unit.hpp"

// 一场战斗
class Combat : public std::enable_shared_from_this<Combat> {
public:
    Combat(uint64 uid);
    ~Combat();

    // 初始化
    bool init(const BattleInitData& data);

// 玩家操作
public:
    // 设置加载结束
    bool setLoadingEnd(uint64 uid);
    // 设置操作技能
    bool setOperateSkill(uint64 uid, uint32 skillId, std::vector<uint64> target);
    // 设置播放完毕
    bool setPlayEnd(uint64 uid);

// 阶段逻辑
private:
    // 加载阶段结束
    bool steploadingEnd();
    // 开始回合制战斗
    void roundStart();
    // 自动攻击
    void autoFight();
    // 回合攻击
    void roundFight();
    // 回合结束
    void roundEnd();
    // 战斗结束
    void combatEnd();

// 战斗逻辑
private:
    // 初始化速度条
    bool initSpeedBar();
    // 跑条计算
    void runSpeedBar();
    // 速度条比较(<)
    static bool compareSpeedBar(const BattleUnitPtr pUnit1, const BattleUnitPtr pUnit2);

// 辅助
public:
    // 获取战斗单元
    BattleUnitPtr getBattleUnit(uint64 uid);
    // 查询阵营单元
    CampUnitPtr getCampUnitByUid(uint64 uid);
    // 查询阵营单元
    CampUnitPtr getCampUnitByCamp(uint32 camp);
private:
    // 获取战斗id
    uint64 getUid() { return m_uid; }
    // 获取当前阶段
    uint32 getStep() { return m_step; }
    // 获取当前状态
    uint32 getState() { return m_state; }
    // 获取回合数
    uint32 getRound() { return m_round; }
    // 检测战斗初始化参数
    bool checkInit(const BattleInitData& data);
    // 检测回合战斗开始
    bool checkRoundStart();
    // 检测回合战斗操作
    bool checkRoundOperate();
    // 检测回合播放结束
    bool checkRoundPlayEnd();
    // 检测战斗结束
    bool checkCombatEnd();
    // 同步战斗数据
    void synCombatData();
    // 获取默认技能目标
    void getDefaultSkillTarget(uint32 skill, std::vector<uint64>& targetVec);
    // 获取阵营内存活战斗单元
    void getAliveUnitByCamp(uint32 camp, std::vector<BattleUnitPtr>& unitVec);
    // 判断阵营全部死亡
    bool isAllDeadByCamp(uint32 camp);

private:
    uint64 m_uid;                                     // 战斗唯一id
    uint32 m_type;                                    // 战斗类型
    uint32 m_step = BATTLE_STEP_INIT;                 // 战斗阶段(默认为初始化阶段)
    uint32 m_state = BATTLE_STATE_INIT;               // 战斗状态
    uint32 m_round = 0;                               // 总回合数
    std::map<uint64, BattleUnitPtr> m_battleUnitMap;  // 战斗单元map(uid:unit)
    std::map<uint32, CampUnitPtr> m_campUnitMap;      // 阵营单元map(camp:unit)
    CombatResult m_result;                            // 战斗结果

private:
    uint32 m_speedBar = 0;                             // 速度条
    BattleUnitPtr m_curOpearteUnit = NULL;             // 当前出手单元
};

typedef std::shared_ptr<Combat> CombatPtr;

#endif

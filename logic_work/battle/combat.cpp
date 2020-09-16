#include "combat.hpp"

#include <cmath>

#include "combat_calc.hpp"
#include "utils/log.hpp"
#include "utils/calc_tool.hpp"
#include "utils/rand.hpp"

USING_NS_UTILS;

Combat::Combat(uint64 uid) : m_uid(uid) {
}

Combat::~Combat() {
    // TODO::暂停计时器 清理内存
}

bool Combat::init(const BattleInitData& data) {
    if (!checkInit(data)) {
        return false;
    }
    for (const auto& value : data.campVec) {
        for (const auto& kv : value.unitMap) {
            BattleUnitPtr pBattleUnit = std::make_shared<BattleUnit>();
            if (!pBattleUnit->init(value.camp, kv.second)) {
                return false;
            }
            m_battleUnitMap[pBattleUnit->getUid()] = pBattleUnit;
        }
        CampUnitPtr pCampUnit = std::make_shared<CampUnit>(value.uid, value.camp, value.type);
        m_campUnitMap[pCampUnit->getCamp()] = pCampUnit;
    }

    m_type = data.param.type;
    m_step = BATTLE_STEP_LOADING;

    // TODO::开启计时器超时后直接进入下一阶段

    // 同步战斗数据
    synCombatData();

    // 尝试开启战斗
    if (checkRoundStart()) {
        return steploadingEnd();
    }
    return true;
}

bool Combat::setLoadingEnd(uint64 uid) {
    if (m_step != BATTLE_STEP_LOADING) {
        log_error("Step[%u] is not BATTLE_STEP_LOADING", m_step);
        return false;
    }
    auto pCampUnit = getCampUnitByUid(uid);
    if (NULL == pCampUnit) {
        log_error("NULL == pCampUnit[%lu]", uid);
        return false;
    }
    pCampUnit->setState(CAMP_UNIT_STATE_LOADING_END);
     // 尝试开启战斗
    if (checkRoundStart()) {
        return steploadingEnd();
    }
    return true;
}

bool Combat::setOperateSkill(uint64 uid, uint32 skillId, std::vector<uint64> target) {
    // TODO::暂时未实现
    return true;
}

bool Combat::setPlayEnd(uint64 uid) {
    // TODO::暂时未实现
    return true;
}

bool Combat::steploadingEnd() {
    log_warn("战斗开始:combat[%lu]~~~~~~~~~~~~~~~~~~~~~~~~", m_uid);
    // 进入回合制战斗阶段
    m_step = BATTLE_STEP_ROUND;

    if (!initSpeedBar()) {
        return false;
    }

    // 战斗开始全部设置操作完毕状态
    for (auto& kv : m_campUnitMap) {
        if (NULL == kv.second) {
            continue;
        }
        kv.second->setState(CAMP_UNIT_STATE_OPERATE_END);
    }

    roundStart();
    return true;
}

void Combat::roundStart() {
    m_round += 1;

    // 回合开始全部设置操作完毕状态
    for (auto& kv : m_campUnitMap) {
        if (NULL == kv.second) {
            continue;
        }
        kv.second->setState(CAMP_UNIT_STATE_OPERATE_END);
    }

    // 跑速度条
    runSpeedBar();
    // 设置操作阵营状态
    if (NULL == m_curOpearteUnit) {
        log_error("Can not find operate unit");
    } else {
        auto pCampUnit = getCampUnitByCamp(m_curOpearteUnit->getCamp());
        if (pCampUnit) {
            pCampUnit->setState(CAMP_UNIT_STATE_OPERATE);
        } else {
            log_error("Can not find camp[%u] unit", m_curOpearteUnit->getCamp());
        }
    }
    // 设置战斗状态
    m_state = BATTLE_STATE_WAITING_OPERATE;
    // 自动战斗
    autoFight();
    // TODO::启动定时器(超时直接释放默认技能)
    // 同步战斗数据(战斗英雄要携带可使用技能)
    synCombatData();
    // 检测是否操作完毕
    if (checkRoundOperate()) {
        roundFight();
    }
}

void Combat::autoFight() {
    if (m_step != BATTLE_STEP_ROUND) {
        log_error("Step[%u] is not BATTLE_STEP_LOADING", m_step);
        return;
    }
    if (m_state != BATTLE_STATE_WAITING_OPERATE) {
        log_error("State[%u] is not BATTLE_STATE_WAITING_OPERATE", m_state);
        return;
    }
    if (NULL == m_curOpearteUnit) {
        log_error("NULL == m_curOpearteUnit");
        return;
    }

    auto pCampUnit = getCampUnitByCamp(m_curOpearteUnit->getCamp());
    if (NULL == pCampUnit) {
        log_error("NULL == pCampUnit by camp[%u]", m_curOpearteUnit->getCamp());
        return;
    }

    if (pCampUnit->getType() == BATTLE_UNIT_TYPE_USER && !pCampUnit->getAuto()) {
        return;
    }
    if (pCampUnit->getState() == CAMP_UNIT_STATE_OPERATE_END) {
        return;
    }

    // 填充默认技能
    uint32 skillId = m_curOpearteUnit->getDefaultSkill();
    std::vector<uint64> targetVec;
    getDefaultSkillTarget(skillId, targetVec);
    m_curOpearteUnit->setCurOperate(skillId, targetVec);
    pCampUnit->setState(CAMP_UNIT_STATE_OPERATE_END);
}

void Combat::roundFight() {
    // 进入战斗计算状态
    m_state = BATTLE_STATE_FIGHTING;

    // 战斗计算
    if (m_curOpearteUnit) {
        CombatCalc::doOperate(m_curOpearteUnit, shared_from_this());
    }

    // 进入播放状态
    m_state = BATTLE_STATE_PLAYING;
    for (auto& kv : m_campUnitMap) {
        if (NULL == kv.second) {
            continue;
        }
        if (kv.second->getType() == BATTLE_UNIT_TYPE_USER) {
            kv.second->setState(CAMP_UNIT_STATE_PLAY);
        } else {
            kv.second->setState(CAMP_UNIT_STATE_PLAY_END);
        }
    }

    // TODO::开启计时器超时自动播放完毕
    // 播放结束
    if (checkRoundPlayEnd()) {
        roundEnd();
    }
}

void Combat::roundEnd() {
    // 清空速度条
    m_curOpearteUnit->setSpeedBar(0);
    // 清空操作
    m_curOpearteUnit->clearCurOperate();
    // 清空本回合操作单元
    m_curOpearteUnit = NULL;

    // 检测战斗结束
    if (checkCombatEnd()) {
        combatEnd();
        return;
    }
    // 新一轮战斗
    roundStart();
}

void Combat::combatEnd() {
    // 战斗结束
    m_step = BATTLE_STEP_END;
    log_warn("战斗结束:combat[%lu] round[%u]~~~~~~~~~~~~~~~", m_uid, m_round);
    if (m_result.ret == COMBAT_RESULT_CAMP2_DEFEAT) {
        log_warn("Camp1 战斗胜利 camp2 全部阵亡.");
    } else if (m_result.ret == COMBAT_RESULT_CAMP1_DEFEAT) {
        log_warn("Camp2 战斗胜利 camp1 全部阵亡.");
    } else if (m_result.ret == COMBAT_RESULT_ALL_DEFEAT) {
        log_warn("Camp1  camp2 全部阵亡.");
    } else if (m_result.ret == COMBAT_RESULT_TIMEOUT) {
        log_warn("战斗超时强制结束");
    }
    for (auto& kv : m_battleUnitMap) {
        if (NULL == kv.second) {
            continue;
        }
        if (kv.second->getCamp() == BATTLE_CAMP_1) {
            log_warn("Camp1 unit[%lu] leftHp[%u]", kv.second->getUid(), kv.second->getHp());
        }
    }
    for (auto& kv : m_battleUnitMap) {
        if (NULL == kv.second) {
            continue;
        }
        if (kv.second->getCamp() == BATTLE_CAMP_2) {
            log_warn("Camp2 unit[%lu] leftHp[%u]", kv.second->getUid(), kv.second->getHp());
        }
    }
}

bool Combat::initSpeedBar() {
    uint32 maxSpeed = 0;
    for (auto& kv : m_battleUnitMap) {
        if (NULL == kv.second) {
            continue;
        }
        if (!kv.second->isAlive()) {
            continue;
        }
        if (maxSpeed < kv.second->getSpeed()) {
            maxSpeed = kv.second->getSpeed();
        }
    }

    if (maxSpeed <= 0) {
        log_error("maxSpeed[%d] <= 0", maxSpeed);
        return false;
    }
    m_speedBar = GET_SPEED_BAR(maxSpeed);
    return true;
}

void Combat::runSpeedBar() {
    // 跑满条最小次数
    uint32 runMinTimes = RUN_SPEED_BAR_INVALID;
    for (auto& kv : m_battleUnitMap) {
        BattleUnitPtr pBattleUnit = kv.second;
        if (NULL == pBattleUnit) {
            continue;
        }
        if (!pBattleUnit->isAlive()) {
            continue;
        }
        if (!pBattleUnit->isCanRunSpeedBar()) {
            continue;
        }
        if (pBattleUnit->getSpeed() <= 0) {
            continue;
        }
        uint32 times = std::ceil(safeSub(m_speedBar, pBattleUnit->getSpeedBarTotal()) * 1.0f / pBattleUnit->getSpeed());
        if (runMinTimes > times) {
            runMinTimes = times;
        }
    }
    if (runMinTimes == RUN_SPEED_BAR_INVALID) {
        log_error("Can not calc runMinTimes~");
        return;
    }

    // 计算每人跑条距离
    for (auto& kv : m_battleUnitMap) {
        BattleUnitPtr pBattleUnit = kv.second;
        if (NULL == pBattleUnit) {
            continue;
        }
        if (!pBattleUnit->isAlive()) {
            continue;
        }
        if (!pBattleUnit->isCanRunSpeedBar()) {
            continue;
        }
        uint32 curSpeedBar = runMinTimes * pBattleUnit->getSpeed() + pBattleUnit->getSpeedBarTotal();
        if (curSpeedBar > m_speedBar) {
            pBattleUnit->setSpeedBar(m_speedBar);
            pBattleUnit->setOverflowSpeedBar(safeSub(curSpeedBar, m_speedBar));
        } else {
            pBattleUnit->setSpeedBar(curSpeedBar);
            pBattleUnit->setOverflowSpeedBar(0);
        }

        if (pBattleUnit->getSpeedBarTotal() < m_speedBar) {
            continue;
        }
        if (m_curOpearteUnit == NULL || compareSpeedBar(m_curOpearteUnit, pBattleUnit)) {
            m_curOpearteUnit = pBattleUnit;
        }
    }
}

bool Combat::compareSpeedBar(const BattleUnitPtr pUnit1, const BattleUnitPtr pUnit2) {
    if (NULL == pUnit1) {
        return true;
    }
    if (NULL == pUnit2) {
        return false;
    }
    if (pUnit1->getSpeedBarTotal() > pUnit2->getSpeedBarTotal()) {
        return false;
    } else if (pUnit1->getSpeedBarTotal() == pUnit2->getSpeedBarTotal()) {
        // 速度相同比较阵营
        if (pUnit1->getCamp() > pUnit2->getCamp()) {
            return false;
        } else if (pUnit1->getCamp() == pUnit2->getCamp()) {
            // 阵营相同比较英雄uid(唯一值)
            if (pUnit1->getUid() > pUnit2->getUid()) {
                return false;
            }
        }
    }
    return true;
}

BattleUnitPtr Combat::getBattleUnit(uint64 uid) {
    auto iter = m_battleUnitMap.find(uid);
    if (iter == m_battleUnitMap.end()) {
        return NULL;
    }
    return iter->second;
}

CampUnitPtr Combat::getCampUnitByUid(uint64 uid) {
    for (auto& kv : m_campUnitMap) {
        if (NULL == kv.second) {
            continue;
        }
        if (kv.second->getUid() == uid) {
            return kv.second;
        }
    }
    return NULL;
}

CampUnitPtr Combat::getCampUnitByCamp(uint32 camp) {
    auto iter = m_campUnitMap.find(camp);
    if (iter == m_campUnitMap.end()) {
        return NULL;
    }
    return iter->second;
}

bool Combat::checkInit(const BattleInitData& data) {
    // TODO::目前只支持双阵营
    if (data.campVec.size() != 2) {
        log_error("Combat[%lu] camp size != 2", m_uid);
        return false;
    }
    // 阵营相同
    if (data.campVec[0].camp == data.campVec[1].camp) {
        log_error("Combat[%lu] have same camp", m_uid);
        return false;
    }
    // 是否具有战斗单元
    if (0 == data.campVec[0].unitMap.size()) {
        log_error("Combat[%lu] camp[%u] have not battle unit", m_uid, data.campVec[0].camp);
        return false;
    }
    if (0 == data.campVec[1].unitMap.size()) {
        log_error("Combat[%lu] camp[%u] have not battle unit", m_uid, data.campVec[1].camp);
        return false;
    }
    return true;
}

bool Combat::checkRoundStart() {
    if (m_step != BATTLE_STEP_LOADING) {
        log_error("Step[%u] is not BATTLE_STEP_LOADING", m_step);
        return false;
    }

    for (auto& kv : m_campUnitMap) {
        if (NULL == kv.second) {
            log_error("NULL == CampUnitPtr[%u]", kv.first);
            return false;
        }
        if (kv.second->getState() != CAMP_UNIT_STATE_LOADING_END) {
            return false;
        }
    }
    return true;
}

bool Combat::checkRoundOperate() {
    if (m_step != BATTLE_STEP_ROUND) {
        log_error("Step[%u] is not BATTLE_STEP_LOADING", m_step);
        return false;
    }
    if (m_state != BATTLE_STATE_WAITING_OPERATE) {
        log_error("State[%u] is not BATTLE_STATE_WAITING_OPERATE", m_state);
        return false;
    }

    for (auto& kv : m_campUnitMap) {
        if (NULL == kv.second) {
            continue;
        }
        if (kv.second->getState() != CAMP_UNIT_STATE_OPERATE_END) {
            return false;
        }
    }
    return true;
}

bool Combat::checkRoundPlayEnd() {
    if (m_step != BATTLE_STEP_ROUND) {
        log_error("Step[%u] is not BATTLE_STEP_ROUND", m_step);
        return false;
    }
    if (m_state != BATTLE_STATE_PLAYING) {
        log_error("State[%u] is not BATTLE_STATE_PLAYING", m_state);
        return false;
    }

    for (auto& kv : m_campUnitMap) {
        if (NULL == kv.second) {
            continue;
        }
        if (kv.second->getState() != CAMP_UNIT_STATE_PLAY_END) {
            return false;
        }
    }
    return true;
}

bool Combat::checkCombatEnd() {
    bool deadCamp1 = isAllDeadByCamp(BATTLE_CAMP_1);
    bool deadCamp2 = isAllDeadByCamp(BATTLE_CAMP_2);
    if (deadCamp1 && deadCamp2) {
        m_result.ret = COMBAT_RESULT_ALL_DEFEAT;
        return true;
    } else if (deadCamp1) {
        m_result.ret = COMBAT_RESULT_CAMP1_DEFEAT;
        return true;
    } else if (deadCamp2) {
        m_result.ret = COMBAT_RESULT_CAMP2_DEFEAT;
        return true;
    } else if (m_round >= MAX_ROUND) {
        m_result.ret = COMBAT_RESULT_TIMEOUT;
        return true;
    }
    return false;
}

void Combat::synCombatData() {
    // TODO::同步战斗数据
}

void Combat::getDefaultSkillTarget(uint32 skill, std::vector<uint64>& targetVec) {
    if (NULL == m_curOpearteUnit) {
        log_error("NULL == m_curOpearteUnit");
        return;
    }
    // 获取敌对阵营
    uint32 targetCamp = m_curOpearteUnit->getCamp() == BATTLE_CAMP_1 ? BATTLE_CAMP_2 : BATTLE_CAMP_1;
    // 普通攻击
    if (BATTLE_SKILL_NORMAL_ATK == skill) {
        std::vector<BattleUnitPtr> unitVec;
        getAliveUnitByCamp(targetCamp, unitVec);
        if (unitVec.size() == 0) {
            log_error("Camp[%u] have not alive unit", targetCamp);
            return;
        }
        uint32 index = Rand::randBetween((uint32)0, (uint32)(unitVec.size() - 1));
        if (index > unitVec.size()) {
            log_error("Rand index[%u] > unitVec size[%u]", index, unitVec.size());
            return;
        }
        targetVec.push_back(unitVec[index]->getUid());
    } else {
        //TODO::其他技能暂未实现
    }
}

void Combat::getAliveUnitByCamp(uint32 camp, std::vector<BattleUnitPtr>& unitVec) {
    for (auto& kv : m_battleUnitMap) {
        if (NULL == kv.second) {
            continue;
        }
        if (kv.second->getCamp() != camp) {
            continue;
        }
        if (!kv.second->isAlive()) {
            continue;
        }
        unitVec.push_back(kv.second);
    }
}

bool Combat::isAllDeadByCamp(uint32 camp) {
    for (auto& kv : m_battleUnitMap) {
        if (NULL == kv.second) {
            continue;
        }
        if (kv.second->getCamp() != camp) {
            continue;
        }
        if (kv.second->isAlive()) {
            return false;
        }
    }
    return true;
}

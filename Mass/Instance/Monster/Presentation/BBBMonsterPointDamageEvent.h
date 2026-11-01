#pragma once

#include "Engine/DamageEvents.h"

/** 怪物通过标准伤害适配器递交的击飞事实 玩家将其转换为自身输入 */
struct ABBB_EVAC_API FBBBMonsterPointDamageEvent final : public FPointDamageEvent
{
    static constexpr int32 ClassID = 0x42424d49;

    /** 怪物本代身份 */
    FGuid SourceId;
    /** 本轮攻击编号 */
    uint32 AttackId = 0;
    /** 玩家应接收的世界空间击飞速度 */
    FVector LaunchVelocity = FVector::ZeroVector;
    /** 本轮击倒的建议物理保持时间 */
    float KnockdownDuration = 0;

    /** @return 本项目怪物命中类型编号 */
    virtual int32 GetTypeID() const override { return ClassID; }
    /** @param InID 待检查类型 @return 是否属于本命中或标准点伤害 */
    virtual bool IsOfType(int32 InID) const override
    {
        return InID == ClassID || FPointDamageEvent::IsOfType(InID);
    }
};

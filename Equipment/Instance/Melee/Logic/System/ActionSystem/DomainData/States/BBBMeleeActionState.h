#pragma once
#include "CoreMinimal.h"
#include "Mass/EntityHandle.h"
/** 单次近战攻击的因果状态 */
struct FBBBMeleeActionState final
{
    /** 每次成立的攻击递增 */
    int32 AttackSequence = 0;
    /** 是否正在等待攻击生命周期结束 */
    bool bAttacking = false;

    /** 持有者操作限制独立于伤害窗口通知 */
    bool bOwnerActionsAllowed = true;
    /** 动画已经开启伤害窗口 */
    bool bContactOpen = false;
    /** 当前攻击的生命周期通知标识 */
    int32 ActionToken = INDEX_NONE;
    /** 当前有效伤害窗口通知标识 */
    int32 ContactToken = INDEX_NONE;
    /** 本次攻击开始时间 */
    double LastAttackTime = -DBL_MAX;
    /** 开窗前后扫掠起点的上一帧位置 */
    FVector PreviousBase = FVector::ZeroVector;
    /** 开窗前后扫掠终点的上一帧位置 */
    FVector PreviousTip = FVector::ZeroVector;
    /** 是否已经取得本次窗口的前帧姿势 */
    bool bHasPreviousPose = false;
    /** 当前攻击已经结算的普通目标 */
    TArray<TWeakObjectPtr<AActor>> HitActors;
    /** 当前攻击已经结算的 Mass 目标 */
    TArray<FMassEntityHandle> HitEntities;
    /** 成立的命中数 用于事实诊断 */
    int32 HitCount = 0;
    /** 当前已成立动作的顺序 */
    uint64 ActionRevision = 0;

private:
    friend struct FBBBMeleeActionDomainState;
    FBBBMeleeActionState() = default;
};

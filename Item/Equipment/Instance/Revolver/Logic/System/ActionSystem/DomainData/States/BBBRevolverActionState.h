#pragma once

#include "CoreMinimal.h"

/** 左轮动作产生并跨帧保留的事实 */
struct FBBBRevolverActionState final
{
    /** 上一帧是否存在开火请求 用于固定半自动方式的按下边沿 */
    bool bPrimaryHeld = false;

    /** 弹匣容量 */
    int32 AmmoCapacity = 0;

    /** 当前装填弹药 */
    int32 LoadedAmmo = 0;

    /** 最近一次确认开火的序号 */
    int32 FireSequence = 0;

    /** 当前换弹流程序号 */
    int32 ReloadSequence = 0;

    /** 最近一次确认开火的世界时间 */
    float LastFireTimeSeconds = -1000.0f;

    /** 动画通知控制的开火限制 不替代弹药与射速检查 */
    bool bFireBlocked = false;

    /** 持有者操作限制 与动画开火通知独立 */
    bool bOwnerActionsAllowed = true;

    /** 当前是否处于换弹流程 */
    bool bIsReloading = false;

    /** 本帧是否由装入新弹匣自然完成换弹 */
    bool bReloadCompletedThisFrame = false;

    /** 本帧是否建立持有关系 */
    bool bEquippedThisFrame = false;

    /** 当前已成立动作的顺序 */
    uint64 ActionRevision = 0;

private:
    friend struct FBBBRevolverActionDomainState;

    FBBBRevolverActionState() = default;
    FBBBRevolverActionState(const FBBBRevolverActionState &) = default;
    FBBBRevolverActionState &operator=(const FBBBRevolverActionState &) = default;
};

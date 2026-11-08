#pragma once

#include "CoreMinimal.h"

/** 冲锋枪动作产生并跨帧保留的事实 */
struct FBBBSMGActionState final
{
    /** 半自动单次点击的短暂有效期 负数表示没有待执行点击 */
    float PendingPrimaryUntilSeconds = -1.0f;

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

    /** 连续射击的下一发计划世界时间 零表示没有活动射击节拍 */
    float NextFireTimeSeconds = 0.0f;

    /** 动画通知控制的开火限制 不替代弹药与射速检查 */
    bool bFireBlocked = false;

    /** 隐藏装备网格并拒绝新的主动输入 不改变内部生命周期 */
    bool bHide = false;
    /** 最新网格显示结果版本 */
    uint64 VisibilityRevision = 0;

    /** 当前是否处于换弹流程 */
    bool bIsReloading = false;

    /** 本帧是否由装入新弹匣自然完成换弹 */
    bool bReloadCompletedThisFrame = false;

    /** 本帧是否建立持有关系 */
    bool bEquippedThisFrame = false;

    /** 当前已成立动作的顺序 */
    uint64 ActionRevision = 0;

private:
    friend struct FBBBSMGActionDomainState;

    FBBBSMGActionState() = default;
    FBBBSMGActionState(const FBBBSMGActionState &) = default;
    FBBBSMGActionState &operator=(const FBBBSMGActionState &) = default;
};

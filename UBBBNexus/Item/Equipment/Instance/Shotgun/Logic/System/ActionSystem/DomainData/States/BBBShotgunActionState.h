#pragma once

#include "CoreMinimal.h"

/** 霰弹枪动作产生并跨帧保留的事实 */
struct FBBBShotgunActionState final
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

    /** 下一次连续开火的计划世界时间 不积攒停火期间的发射次数 */
    float NextFireTimeSeconds = 0.0f;

    /** 动画通知控制的开火限制 不替代弹药与射速检查 */
    bool bFireBlocked = false;

    /** 隐藏装备网格并拒绝新的主动输入 不改变内部生命周期 */
    bool bHide = false;
    /** 最新网格显示结果版本 */
    uint64 VisibilityRevision = 0;

    /** 当前是否处于换弹流程 */
    bool bIsReloading = false;

    /** 已结束装填 正在等待收手动画结束 此期间不能射击或重新换弹 */
    bool bReloadFinishing = false;

    /** 装填中收到的有效开火边沿 等待本轮收手结束后执行一次 */
    bool bFireAfterReload = false;

    /** 本帧是否由装入新弹匣自然完成换弹 */
    bool bReloadCompletedThisFrame = false;

    /** 本帧是否建立持有关系 */
    bool bEquippedThisFrame = false;

    /** 当前已成立动作的顺序 */
    uint64 ActionRevision = 0;

private:
    friend struct FBBBShotgunActionDomainState;

    FBBBShotgunActionState() = default;
    FBBBShotgunActionState(const FBBBShotgunActionState &) = default;
    FBBBShotgunActionState &operator=(const FBBBShotgunActionState &) = default;
};

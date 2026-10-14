#pragma once
#include "CoreMinimal.h"

/** 手雷Use领域持久状态 */
struct FBBBGrenadeUseState final
{
    /** 持有者当前许可 */
    bool bAllowed = false;
    /** 上一帧是否持续收到主行为 按下边缘只使用一次 */
    bool bPrimaryHeld = false;
    /** 当前使用动作 */
    bool bUsing = false;
    /** 当前手持模型是否已释放 */
    bool bReleased = false;
    /** 已开始的动作序号 */
    int32 UseSerial = 0;
    /** 已成立的释放序号 */
    int32 ReleaseSerial = 0;
    /** 房主已生成投掷物的释放序号 */
    int32 SpawnedReleaseSerial = 0;
    /** 已还原的网络结果顺序 */
    uint64 ReceivedRevision = 0;
    /** 已成立的释放变换 */
    /** 释放时持有者速度 */
    FVector InheritedVelocity = FVector::ZeroVector;
    FTransform LaunchTransform = FTransform::Identity;
};

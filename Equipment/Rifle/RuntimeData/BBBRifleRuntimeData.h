#pragma once

#include "CoreMinimal.h"

/** 步枪实例跨帧保留的全部运行时事实 */
struct FBBBRifleRuntimeData final
{
    /** 弹匣容量 */
    int32 AmmoCapacity = 0;

    /** 当前装填弹药 */
    int32 LoadedAmmo = 0;

    /** 最近一次确认开火的序号 */
    int32 FireSequence = 0;

    /** 当前换弹流程序号 */
    int32 ReloadSequence = INDEX_NONE;

    /** 最近一次确认开火的世界时间 */
    float LastFireTimeSeconds = -1000.0f;

    /** 当前是否处于换弹流程 */
    bool bIsReloading = false;

    /** 当前换弹流程是否已经取下旧弹匣 */
    bool bMagazineDetached = false;
};

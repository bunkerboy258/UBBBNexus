#pragma once

#include "CoreMinimal.h"

/** 步枪动作领域本帧待处理输入与镜像结果 */
struct FBBBRifleActionInputState final
{
    /** 本帧是否收到装备表现请求 */
    bool bEquipRequested = false;

    /** 本帧是否收到动画禁止开火请求 */
    bool bBlockFireRequested = false;

    /** 本帧是否收到动画允许开火请求 */
    bool bAllowFireRequested = false;

    /** 本帧是否收到开火请求 */
    bool bPrimaryRequested = false;

    /** 本帧是否收到换弹请求 */
    bool bReloadRequested = false;

    /** 本帧是否收到装入弹匣通知 */
    bool bLoadMagazineRequested = false;

    /** 本帧是否收到结束换弹通知 */
    bool bInterruptReloadRequested = false;

    /** 本帧是否收到镜像当前结果 */
    bool bHasAuthorityFact = false;

    /** 镜像当前装填弹药 */
    int32 LoadedAmmo = 0;

    /** 镜像当前累计开火次数 */
    int32 FireSequence = 0;

    /** 镜像当前换弹流程序号 */
    int32 ReloadSequence = 0;

    /** 镜像当前是否处于换弹流程 */
    bool bIsReloading = false;

    /** 当前动画是否禁止开火 */
    bool bFireBlocked = false;

private:
    friend struct FBBBRifleActionDomainState;

    FBBBRifleActionInputState() = default;
};

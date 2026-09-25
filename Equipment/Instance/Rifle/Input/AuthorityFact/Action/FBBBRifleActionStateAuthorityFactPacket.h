#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ActionSystem/DomainData/States/BBBRifleActionInputState.h"

/** 步枪镜像只还原当前结果 不重放动作历史 */
struct FBBBRifleActionStateAuthorityFactPacket final
{
    /** 当前装填弹量 */
    int32 LoadedAmmo = 0;

    /** 当前累计开火次数 */
    int32 FireSequence = 0;

    /** 当前换弹流程序号 */
    int32 ReloadSequence = 0;

    /** 当前是否正在换弹 */
    bool bIsReloading = false;

    /** 当前弹匣是否已经卸下 */
    bool bMagazineDetached = false;

    /** @return 镜像结果自身的数据约束是否有效 */
    bool IsValid() const
    {
        return LoadedAmmo >= 0 && FireSequence >= 0 && ReloadSequence >= 0 && (!bMagazineDetached || bIsReloading);
    }

    /** @return 镜像结果是否可写入本帧状态 */
    bool CanApply() const
    {
        return true;
    }

    /**
     * 将当前结果写入动作领域状态
     * @param State	步枪动作输入状态
     * @return 无
     */
    void Apply(FBBBRifleActionInputState &State) const
    {
        State.bHasAuthorityFact = true;
        State.LoadedAmmo = LoadedAmmo;
        State.FireSequence = FireSequence;
        State.ReloadSequence = ReloadSequence;
        State.bIsReloading = bIsReloading;
        State.bMagazineDetached = bMagazineDetached;
    }
};

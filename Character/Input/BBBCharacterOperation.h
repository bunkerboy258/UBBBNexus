#pragma once

#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/DomainData/States/BBBCharacterOperationState.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"

/** 输入包共享的角色操作状态规则 */
namespace BBBCharacterOperation
{
    /** @return 当前是否存在进行中的换弹 */
    inline bool IsReloadInProgress(const FBBBCharacterOperationState &State)
    {
        return State.ReloadSequence > 0;
    }

    /** @return 本帧是否存在待切换装备 */
    inline bool IsEquipmentSwitchPending(const FBBBCharacterOperationState &State)
    {
        return State.SelectedEquipment != nullptr;
    }

    /**
     * 判断序号是否属于当前换弹
     * @param State     操作状态
     * @param Sequence  待检查序号
     * @return 是否属于当前换弹
     */
    inline bool IsCurrentReloadSequence(
        const FBBBCharacterOperationState &State,
        const int32 Sequence)
    {
        return State.ReloadSequence > 0 && Sequence == State.ReloadSequence;
    }

    /**
     * 判断序号是否属于本帧取消的换弹
     * @param State     操作状态
     * @param Sequence  待检查序号
     * @return 是否属于本帧取消的换弹
     */
    inline bool IsCancelledReloadSequence(
        const FBBBCharacterOperationState &State,
        const int32 Sequence)
    {
        return State.CancelReloadSequence > 0 && Sequence == State.CancelReloadSequence;
    }

    /**
     * 取消进行中的换弹
     * @param State 需要维护的操作状态
     * @return 无
     */
    inline void CancelReload(FBBBCharacterOperationState &State)
    {
        State.CancelReloadSequence = State.ReloadSequence;
        State.LastCompletedReloadSequence = FMath::Max(
            State.LastCompletedReloadSequence,
            State.ReloadSequence);
        State.ReloadSequence = INDEX_NONE;
        State.ReloadEquipment.Reset();
        State.bMagazineDetached = false;
        State.bEndQueued = false;
    }

    /**
     * 开启换弹追踪
     * @param State 操作状态
     * @param Sequence 换弹序号
     * @param FactEquipmentId 事实装备标识
     * @param ActiveEquipment 当前装备
     * @param ActiveEquipmentId 当前装备标识
     * @return 无
     */
    inline void TrackReloadStarted(
        FBBBCharacterOperationState &State,
        const int32 Sequence,
        const FName FactEquipmentId,
        ABBBEquipment *ActiveEquipment,
        const FName ActiveEquipmentId)
    {
        if (!ActiveEquipment || FactEquipmentId != ActiveEquipmentId)
        {
            return;
        }

        if (Sequence <= State.LastCompletedReloadSequence || Sequence <= State.ReloadSequence)
        {
            return;
        }

        State.ReloadSequence = Sequence;
        State.ReloadEquipment = ActiveEquipment;
        State.bMagazineDetached = false;
        State.bEndQueued = false;
    }

    /**
     * 记录弹匣已经卸下
     * @param State 操作状态
     * @param Sequence 换弹序号
     * @param FactEquipmentId 事实装备标识
     * @param ActiveEquipmentId 当前装备标识
     * @return 无
     */
    inline void TrackMagazineDetached(
        FBBBCharacterOperationState &State,
        const int32 Sequence,
        const FName FactEquipmentId,
        const FName ActiveEquipmentId)
    {
        if (FactEquipmentId != ActiveEquipmentId || Sequence != State.ReloadSequence)
        {
            return;
        }

        State.bMagazineDetached = true;
    }

    /**
     * 完结换弹追踪
     * @param State 操作状态
     * @param Sequence 换弹序号
     * @param FactEquipmentId 事实装备标识
     * @param ActiveEquipmentId 当前装备标识
     * @param bCancelled 是否取消结束
     * @return 无
     */
    inline void TrackReloadFinished(
        FBBBCharacterOperationState &State,
        const int32 Sequence,
        const FName FactEquipmentId,
        const FName ActiveEquipmentId,
        const bool bCancelled)
    {
        if (FactEquipmentId != ActiveEquipmentId || Sequence != State.ReloadSequence)
        {
            return;
        }

        if (bCancelled)
        {
            State.CancelReloadSequence = Sequence;
        }

        State.LastCompletedReloadSequence = Sequence;
        State.ReloadSequence = INDEX_NONE;
        State.ReloadEquipment.Reset();
        State.bMagazineDetached = false;
        State.bEndQueued = false;
    }
}

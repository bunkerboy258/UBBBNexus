#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"
#include "Animation/AnimMontage.h"

void FBBBCharacterMontagePacket::BeginFrame(FBBBCharacterRuntimeData &Data)
{
    FBBBAnimationRuntimeData &Animation = Data.Animation;
    const FBBBCharacterOperationState &Operation = Data.Operation;
    if (Animation.Slots.IsEmpty())
    {
        for (const FName Slot : {FName(TEXT("FullBody")), FName(TEXT("UpperBody")),
            FName(TEXT("FullBodyAdditivePreAim")), FName(TEXT("UpperBodyAdditive")),
            FName(TEXT("AdditiveHitReact"))})
        {
            FBBBCharacterMontageSlotState &State = Animation.Slots.AddDefaulted_GetRef();
            State.Slot = Slot;
        }
    }

    // 切换装备或中断换弹时撤销相关槽位期望 播放组件随后按修订号处理
    for (FBBBCharacterMontageSlotState &Slot : Animation.Slots)
    {
        if (Operation.SelectedEquipment
            || (Operation.CancelReloadSequence > 0 && Slot.Desired.bReload
                && Slot.Desired.Sequence == Operation.CancelReloadSequence))
        {
            Slot.Desired = FBBBCharacterMontagePacket();
            Slot.Revision = 0;
        }
    }
}

bool FBBBCharacterMontagePacket::CanApply(const FBBBCharacterRuntimeData &Data) const
{
    const FBBBCharacterOperationState &Operation = Data.Operation;
    if (Operation.SelectedEquipment || !IsValid(Montage))
    {
        return false;
    }
    if (bReload && (Sequence == Operation.CancelReloadSequence
        || (!Operation.bRestoreMode && Sequence != Operation.ReloadSequence)))
    {
        return false;
    }

    bool bValidSlots = !Montage->SlotAnimTracks.IsEmpty();
    for (const FSlotAnimationTrack &Track : Montage->SlotAnimTracks)
    {
        bValidSlots &= Data.Animation.Slots.ContainsByPredicate([&Track](const FBBBCharacterMontageSlotState &Slot)
        {
            return Slot.Slot == Track.SlotName;
        });
    }
    return ensureMsgf(bValidSlots, TEXT("[UBBBC]Montage uses unsupported slots Asset=%s"),
        *Montage->GetPathName());
}

void FBBBCharacterMontagePacket::Apply(FBBBCharacterRuntimeData &Data) const
{
    FBBBAnimationRuntimeData &Animation = Data.Animation;

    // 同组蒙太奇遵循引擎互斥规则 多轨道共享一次修订号
    for (FBBBCharacterMontageSlotState &Slot : Animation.Slots)
    {
        if (Slot.Desired.Montage && Slot.Desired.Montage->GetGroupName() == Montage->GetGroupName())
        {
            Slot.Desired = FBBBCharacterMontagePacket();
            Slot.Revision = 0;
        }
    }
    const uint64 Revision = Animation.NextRevision++;
    for (const FSlotAnimationTrack &Track : Montage->SlotAnimTracks)
    {
        for (FBBBCharacterMontageSlotState &Slot : Animation.Slots)
        {
            if (Slot.Slot == Track.SlotName)
            {
                Slot.Desired = *this;
                Slot.Revision = Revision;
            }
        }
    }
}

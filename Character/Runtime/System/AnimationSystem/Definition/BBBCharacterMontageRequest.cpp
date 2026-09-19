#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBCharacterMontageRequest.h"

#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Definition/BBBCharacterParseState.h"
#include "Animation/AnimMontage.h"

void FBBBCharacterMontagePacket::BeginFrame(FBBBAnimationRuntimeData &Animation, const FBBBCharacterParseState &Operation)
{
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
        if (Operation.IsEquipmentSwitchPending()
            || (Operation.IsCancelledReloadSequence(Slot.Desired.Sequence) && Slot.Desired.bReload))
        {
            Slot.Desired = FBBBCharacterMontagePacket();
            Slot.Revision = 0;
        }
    }
}

bool FBBBCharacterMontagePacket::CanApply(const FBBBAnimationRuntimeData &Animation, const FBBBCharacterParseState &Operation) const
{
    if (Operation.IsEquipmentSwitchPending() || !IsValid(Montage))
    {
        return false;
    }

    // 换弹蒙太奇只接受当前换弹序号 刚取消的序号与还原模式外的陌生序号都拒收
    if (bReload && (Operation.IsCancelledReloadSequence(Sequence)
        || (!Operation.IsRestoreMode() && !Operation.IsCurrentReloadSequence(Sequence))))
    {
        return false;
    }

    bool bValidSlots = !Montage->SlotAnimTracks.IsEmpty();
    for (const FSlotAnimationTrack &Track : Montage->SlotAnimTracks)
    {
        bValidSlots &= Animation.Slots.ContainsByPredicate([&Track](const FBBBCharacterMontageSlotState &Slot)
        {
            return Slot.Slot == Track.SlotName;
        });
    }
    return ensureMsgf(bValidSlots, TEXT("[UBBBC]Montage uses unsupported slots Asset=%s"),
        *Montage->GetPathName());
}

void FBBBCharacterMontagePacket::Apply(FBBBAnimationRuntimeData &Animation) const
{
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

#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBCharacterMontageRequest.h"

#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Definition/BBBCharacterParseState.h"
#include "Animation/AnimMontage.h"

void FBBBCharacterMontagePacket::BeginFrame(FBBBAnimationRuntimeData &Animation, const FBBBCharacterParseState &Operation)
{
    // 切换装备或中断换弹时撤销相关槽位期望 播放组件随后按修订号处理
    Animation.Slots.ForEach([&Operation](FBBBCharacterMontageSlotState &Slot)
    {
        if (Operation.IsEquipmentSwitchPending()
            || (Operation.IsCancelledReloadSequence(Slot.Desired.Sequence) && Slot.Desired.bReload))
        {
            Slot.Desired = FBBBCharacterMontagePacket();
            Slot.Revision = 0;
        }
    });
}

bool FBBBCharacterMontagePacket::CanApply(
    const FBBBAnimationRuntimeData &Animation,
    const FBBBCharacterParseState &Operation,
    const FName Slot) const
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

    const bool bKnownSlot = Animation.Slots.Find(Slot) != nullptr;
    const bool bConfiguredTrack = Montage->SlotAnimTracks.ContainsByPredicate(
        [Slot](const FSlotAnimationTrack &Track)
        {
            return Track.SlotName == Slot;
        });

    if (!ensureMsgf(bKnownSlot && bConfiguredTrack,
        TEXT("[UBBBC]Montage slot input does not match asset configuration Asset=%s Slot=%s"),
        *Montage->GetPathName(),
        *Slot.ToString()))
    {
        return false;
    }

    return true;
}

void FBBBCharacterMontagePacket::Apply(
    FBBBAnimationRuntimeData &Animation,
    const FName SlotName) const
{
    uint64 SharedRevision = 0;
    Animation.Slots.ForEach([this, &SharedRevision](const FBBBCharacterMontageSlotState &Slot)
    {
        if (Slot.Desired.Montage == Montage
            && Slot.Desired.Sequence == Sequence
            && Slot.Revision != 0)
        {
            SharedRevision = Slot.Revision;
        }
    });

    // 同一蒙太奇的多个槽位复用修订号，动画系统只创建一个播放实例
    if (SharedRevision != 0)
    {
        FBBBCharacterMontageSlotState *Target = Animation.Slots.Find(SlotName);

        if (ensureMsgf(Target, TEXT("[UBBBC]Configured montage slot state is missing")))
        {
            Target->Desired = *this;
            Target->Revision = SharedRevision;
        }
        return;
    }

    // 新播放遵循引擎蒙太奇组互斥规则，先撤销同组旧期望
    Animation.Slots.ForEach([this](FBBBCharacterMontageSlotState &Slot)
    {
        if (Slot.Desired.Montage && Slot.Desired.Montage->GetGroupName() == Montage->GetGroupName())
        {
            Slot.Desired = FBBBCharacterMontagePacket();
            Slot.Revision = 0;
        }
    });

    FBBBCharacterMontageSlotState *Target = Animation.Slots.Find(SlotName);

    if (!ensureMsgf(Target, TEXT("[UBBBC]Configured montage slot state is missing")))
    {
        return;
    }

    Target->Desired = *this;
    Target->Revision = Animation.NextRevision++;
}

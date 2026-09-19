#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Processors/BBBCharacterAnimationActionProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Playback/BBBMontagePlayback.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"

void FBBBCharacterAnimationActionProcessor::Update(
    UBBBAnimInstance &AnimInstance, FBBBAnimationRuntimeData &AnimationData) const
{
    // 播放结束只清理属于自身修订号的槽 新播放不能被旧回调清空
    for (UBBBMontagePlayback *Playback : AnimationData.Playbacks)
    {
        if (!Playback)
        {
            continue;
        }
        if (Playback->IsFinished())
        {
            AnimationData.Slots.ForEach([Playback](FBBBCharacterMontageSlotState &Slot)
            {
                if (Slot.Revision == Playback->GetRevision())
                {
                    Slot.Desired = FBBBCharacterMontagePacket();
                    Slot.Revision = 0;
                }
            });
        }
        const bool bDesired = AnimationData.Slots.ContainsRevision(Playback->GetRevision());
        if (!bDesired)
        {
            Playback->Stop();
        }
    }
    AnimationData.Playbacks.RemoveAll([](UBBBMontagePlayback *Playback)
    {
        return !Playback || Playback->IsFinished();
    });

    // 同一蒙太奇的多个槽共享修订号 仅创建一次引擎播放实例
    AnimationData.Slots.ForEach([&AnimInstance, &AnimationData](const FBBBCharacterMontageSlotState &Slot)
    {
        if (!Slot.Desired.Montage || Slot.Revision == 0)
        {
            return;
        }
        const bool bPlaying = AnimationData.Playbacks.ContainsByPredicate(
            [&Slot](const UBBBMontagePlayback *Playback)
            {
                return Playback && Playback->GetRevision() == Slot.Revision;
            });
        if (bPlaying)
        {
            return;
        }
        UBBBMontagePlayback *Playback = NewObject<UBBBMontagePlayback>(&AnimInstance);
        AnimationData.Playbacks.Add(Playback);
        Playback->Start(AnimInstance, *Slot.Desired.Montage, Slot.Desired.PlayRate,
            Slot.Desired.Sequence, Slot.Revision, Slot.Desired.bReload);
    });
}

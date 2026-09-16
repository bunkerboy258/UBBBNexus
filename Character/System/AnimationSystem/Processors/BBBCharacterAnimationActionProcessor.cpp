#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationActionProcessor.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/Packets/BBBCharacterMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Playback/BBBReloadMontagePlayback.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"

void FBBBCharacterAnimationActionProcessor::Update(
    UBBBAnimInstance &AnimInstance,
    FBBBAnimationRuntimeData &AnimationData) const
{
    for (const int32 Sequence : AnimationData.CancelledReloadSequences)
    {
        for (UBBBReloadMontagePlayback *Playback : AnimationData.ReloadPlaybacks)
        {
            if (Playback)
            {
                Playback->Cancel(Sequence);
            }
        }
    }
    AnimationData.CancelledReloadSequences.Reset();

    AnimationData.ReloadPlaybacks.RemoveAll([](const TObjectPtr<UBBBReloadMontagePlayback> &Playback)
    {
        return !Playback || Playback->IsFinished();
    });

    TArray<FBBBCharacterMontagePacket> Packets = MoveTemp(AnimationData.MontageQueue);
    AnimationData.MontageQueue.Reset();

    for (const FBBBCharacterMontagePacket &Packet : Packets)
    {
        if (!ensureMsgf(
            Packet.Montage && FMath::IsFinite(Packet.PlayRate) && Packet.PlayRate > 0.0f,
            TEXT("[UBBBC]Queued equipment montage packet is invalid")))
        {
            continue;
        }

        if (Packet.bReload)
        {
            UBBBReloadMontagePlayback *Playback = NewObject<UBBBReloadMontagePlayback>(&AnimInstance);
            AnimationData.ReloadPlaybacks.Add(Playback);
            Playback->Start(AnimInstance, *Packet.Montage, Packet.PlayRate, Packet.Sequence);
            continue;
        }

        AnimInstance.ExecuteEquipmentActionMontage(
            Packet.Montage,
            Packet.PlayRate);
    }
}

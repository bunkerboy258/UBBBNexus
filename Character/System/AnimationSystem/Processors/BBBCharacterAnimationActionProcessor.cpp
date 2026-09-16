#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationActionProcessor.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/Packets/BBBCharacterMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"

void FBBBCharacterAnimationActionProcessor::Update(
    UBBBAnimInstance &AnimInstance,
    FBBBAnimationRuntimeData &AnimationData) const
{
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

        AnimInstance.ExecuteEquipmentActionMontage(
            Packet.Montage,
            Packet.PlayRate);
    }
}

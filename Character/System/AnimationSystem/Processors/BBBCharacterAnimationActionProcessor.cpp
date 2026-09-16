#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationActionProcessor.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/Packets/BBBCharacterMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"
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

        EBBBCharacterActionType CharacterAction = EBBBCharacterActionType::None;
        switch (Packet.ActionType)
        {
        case EBBBEquipmentActionType::Equip:
            CharacterAction = EBBBCharacterActionType::Equip;
            break;

        case EBBBEquipmentActionType::Fire:
            CharacterAction = EBBBCharacterActionType::Fire;
            break;

        case EBBBEquipmentActionType::Reload:
            CharacterAction = EBBBCharacterActionType::Reload;
            break;

        default:
            ensureMsgf(false, TEXT("[UBBBC]Queued equipment montage has no supported action type"));
            continue;
        }

        AnimInstance.ExecuteEquipmentActionMontage(
            CharacterAction,
            Packet.Montage,
            Packet.PlayRate);
    }
}

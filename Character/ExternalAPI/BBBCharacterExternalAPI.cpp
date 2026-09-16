#include "BBBWork/UBBBNexus/Character/ExternalAPI/BBBCharacterExternalAPI.h"

#include "BBBWork/UBBBNexus/Character/ExternalAPI/Packets/BBBCharacterMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "Animation/AnimMontage.h"

void FBBBCharacterExternalAPI::Initialize(FBBBAnimationRuntimeData &InAnimationData)
{
    AnimationData = &InAnimationData;
}

bool FBBBCharacterExternalAPI::SubmitEquipmentMontage(
    const EBBBEquipmentActionType ActionType,
    UAnimMontage *Montage,
    const float PlayRate)
{
    if (!ensureMsgf(AnimationData, TEXT("[UBBBC]Character montage queue is not initialized")))
    {
        return false;
    }

    if (!ensureMsgf(
        ActionType != EBBBEquipmentActionType::None && Montage && FMath::IsFinite(PlayRate) && PlayRate > 0.0f,
        TEXT("[UBBBC]Equipment montage contribution is invalid")))
    {
        return false;
    }

    FBBBCharacterMontagePacket Packet;
    Packet.ActionType = ActionType;
    Packet.Montage = Montage;
    Packet.PlayRate = PlayRate;
    AnimationData->MontageQueue.Add(MoveTemp(Packet));
    return true;
}

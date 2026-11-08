#include "BBBWork/UBBBNexus/Notify/Character/Logic/BBBCharacterEquipmentHideLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterEquipmentHideLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimNotifyQueue.h"
void UBBBCharacterEquipmentHideLogicAnimNotifyState::NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *,
    float, const FAnimNotifyEventReference &Reference)
{
    auto *Character = MeshComp ? Cast<ABBBCharacter>(MeshComp->GetOwner()) : nullptr;
    if (!Character || Character->IsNetworkMirror() || !Reference.IsActiveContext())
    {
        return;
    }
    Character->SubmitInput(FBBBCharacterEquipmentHideLocalControlPacket{{Character->GetItemGeneration()}});
}

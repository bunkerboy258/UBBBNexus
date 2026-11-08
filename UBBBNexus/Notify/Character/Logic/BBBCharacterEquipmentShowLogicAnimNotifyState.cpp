#include "BBBWork/UBBBNexus/Notify/Character/Logic/BBBCharacterEquipmentShowLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterEquipmentShowLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimNotifyQueue.h"
void UBBBCharacterEquipmentShowLogicAnimNotifyState::NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *,
    float, const FAnimNotifyEventReference &Reference)
{
    auto *Character = MeshComp ? Cast<ABBBCharacter>(MeshComp->GetOwner()) : nullptr;
    if (!Character || Character->IsNetworkMirror() || !Reference.IsActiveContext())
    {
        return;
    }
    Character->SubmitInput(FBBBCharacterEquipmentShowLocalControlPacket{{Character->GetItemGeneration()}});
}

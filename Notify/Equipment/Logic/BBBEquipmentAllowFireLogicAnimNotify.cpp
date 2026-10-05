#include "BBBWork/UBBBNexus/Notify/Equipment/Logic/BBBEquipmentAllowFireLogicAnimNotify.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBEquipmentAllowFireLogicAnimNotify::Notify(USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase*, const FAnimNotifyEventReference&)
{
    if (!MeshComp) { return; }
    ABBBEquipment* Equipment = Cast<ABBBEquipment>(MeshComp->GetOwner());
    if (ABBBCharacter* Character = Cast<ABBBCharacter>(MeshComp->GetOwner()))
    {
        Equipment = Character->GetActiveEquipment();
    }
    if (IsValid(Equipment) && Equipment->IsEquipped() && !Equipment->IsMirror())
    {
        Equipment->SubmitInput(FBBBEquipmentAllowFireLocalControlPacket{});
    }
}

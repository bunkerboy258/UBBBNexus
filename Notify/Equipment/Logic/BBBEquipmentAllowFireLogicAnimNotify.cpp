#include "BBBWork/UBBBNexus/Notify/Equipment/Logic/BBBEquipmentAllowFireLogicAnimNotify.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Fire/FBBBRifleAllowFireLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBEquipmentAllowFireLogicAnimNotify::Notify(USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase*, const FAnimNotifyEventReference&)
{
    if (!MeshComp) { return; }
    ABBBRifleEquipment* Equipment = Cast<ABBBRifleEquipment>(MeshComp->GetOwner());
    if (IsValid(Equipment) && Equipment->IsEquipped() && !Equipment->IsMirror())
    {
        Equipment->SubmitInput(FBBBRifleAllowFireLocalControlPacket{});
    }
}

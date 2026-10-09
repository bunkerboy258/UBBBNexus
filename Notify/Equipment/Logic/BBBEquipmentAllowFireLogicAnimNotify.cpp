#include "BBBWork/UBBBNexus/Notify/Equipment/Logic/BBBEquipmentAllowFireLogicAnimNotify.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBEquipmentAllowFireLogicAnimNotify::Notify(USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase*, const FAnimNotifyEventReference&)
{
    if (!MeshComp)
    {
        return;
    }
    ABBBEquipment* Equipment = Cast<ABBBEquipment>(MeshComp->GetOwner());
    if (IsValid(Equipment) && Equipment->IsEquipped() && !Equipment->IsMirror())
    {
        Equipment->SubmitInput(FBBBEquipmentAllowFireLocalControlPacket{});
    }
}

#include "BBBWork/UBBBNexus/Notify/Equipment/Logic/BBBEquipmentBlockFireLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Fire/FBBBEquipmentAllowFireLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBEquipmentBlockFireLogicAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase*, float, const FAnimNotifyEventReference&)
{
    if (!MeshComp)
    {
        return;
    }
    ABBBEquipment* Equipment = Cast<ABBBEquipment>(MeshComp->GetOwner());
    if (!IsValid(Equipment) || !Equipment->IsEquipped() || Equipment->IsMirror())
    {
        return;
    }
    for (auto It = Recipients.CreateIterator(); It; ++It)
    {
        if (!It.Key().IsValid() || !It.Value().IsValid())
        {
            It.RemoveCurrent();
        }
    }
    if (Equipment->SubmitInput(FBBBEquipmentBlockFireLocalControlPacket{}))
    {
        Recipients.Add(MeshComp, Equipment);
    }
}

void UBBBEquipmentBlockFireLogicAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase*, const FAnimNotifyEventReference&)
{
    TWeakObjectPtr<ABBBEquipment> Recipient;
    if (!Recipients.RemoveAndCopyValue(MeshComp, Recipient))
    {
        return;
    }
    ABBBEquipment* Equipment = Recipient.Get();
    if (IsValid(Equipment) && Equipment->IsEquipped() && !Equipment->IsMirror())
    {
        Equipment->SubmitInput(FBBBEquipmentAllowFireLocalControlPacket{});
    }
}

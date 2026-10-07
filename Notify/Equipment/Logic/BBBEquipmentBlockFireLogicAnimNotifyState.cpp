#include "BBBWork/UBBBNexus/Notify/Equipment/Logic/BBBEquipmentBlockFireLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Fire/FBBBRifleBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Fire/FBBBRifleAllowFireLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBEquipmentBlockFireLogicAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase*, float, const FAnimNotifyEventReference&)
{
    if (!MeshComp) { return; }
    ABBBRifleEquipment* Equipment = Cast<ABBBRifleEquipment>(MeshComp->GetOwner());
    if (!IsValid(Equipment) || !Equipment->IsEquipped() || Equipment->IsMirror()) { return; }
    for (auto It = Recipients.CreateIterator(); It; ++It)
    {
        if (!It.Key().IsValid() || !It.Value().IsValid()) { It.RemoveCurrent(); }
    }
    if (Equipment->SubmitInput(FBBBRifleBlockFireLocalControlPacket{}))
    {
        Recipients.Add(MeshComp, Equipment);
    }
}

void UBBBEquipmentBlockFireLogicAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase*, const FAnimNotifyEventReference&)
{
    TWeakObjectPtr<ABBBRifleEquipment> Recipient;
    if (!Recipients.RemoveAndCopyValue(MeshComp, Recipient)) { return; }
    ABBBRifleEquipment* Equipment = Recipient.Get();
    if (IsValid(Equipment) && Equipment->IsEquipped() && !Equipment->IsMirror())
    {
        Equipment->SubmitInput(FBBBRifleAllowFireLocalControlPacket{});
    }
}

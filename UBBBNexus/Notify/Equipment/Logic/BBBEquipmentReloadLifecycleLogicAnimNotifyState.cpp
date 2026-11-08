#include "BBBWork/UBBBNexus/Notify/Equipment/Logic/BBBEquipmentReloadLifecycleLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBEquipmentReloadLifecycleLogicAnimNotifyState::NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *,
    const FAnimNotifyEventReference &)
{
    ABBBEquipment *Equipment = MeshComp ? Cast<ABBBEquipment>(MeshComp->GetOwner()) : nullptr;
    if (!Equipment || !Equipment->IsEquipped() || Equipment->IsMirror())
    {
        return;
    }

    Equipment->SubmitInput(FBBBEquipmentInterruptReloadLocalControlPacket<>{});
}

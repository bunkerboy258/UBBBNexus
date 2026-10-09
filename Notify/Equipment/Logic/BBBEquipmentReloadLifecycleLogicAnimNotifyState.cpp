#include "BBBWork/UBBBNexus/Notify/Equipment/Logic/BBBEquipmentReloadLifecycleLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentInterruptReloadLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBEquipmentReloadLifecycleLogicAnimNotifyState::NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *,
    const FAnimNotifyEventReference &)
{
    ABBBEquipment *Equipment = MeshComp ? Cast<ABBBEquipment>(MeshComp->GetOwner()) : nullptr;
    if (!Equipment || !Equipment->IsEquipped() || Equipment->IsMirror())
    {
        return;
    }

    ensureMsgf(Equipment->SubmitInput(FBBBEquipmentInterruptReloadLocalControlPacket{}),
        TEXT("装备换弹生命周期通知提交失败 %s"), *Equipment->GetName());
}

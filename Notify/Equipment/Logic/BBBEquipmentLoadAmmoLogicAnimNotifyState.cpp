#include "BBBWork/UBBBNexus/Notify/Equipment/Logic/BBBEquipmentLoadAmmoLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Input/LocalControl/Reload/FBBBEquipmentLoadAmmoLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBEquipmentLoadAmmoLogicAnimNotifyState::NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *, float,
    const FAnimNotifyEventReference &)
{
    ABBBEquipment *Equipment = MeshComp ? Cast<ABBBEquipment>(MeshComp->GetOwner()) : nullptr;
    if (!Equipment || !Equipment->IsEquipped() || Equipment->IsMirror())
    {
        return;
    }

    ensureMsgf(Equipment->SubmitInput(FBBBEquipmentLoadAmmoLocalControlPacket{}),
        TEXT("装备装入弹药通知提交失败 %s"), *Equipment->GetName());
}

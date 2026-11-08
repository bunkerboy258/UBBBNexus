#include "BBBWork/UBBBNexus/Notify/Equipment/Logic/BBBShotgunReloadCycleLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Reload/FBBBShotgunReloadCycleEndLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Reload/FBBBShotgunInterruptReloadLocalControlPacket.h"
#include "Animation/AnimNotifyLibrary.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBShotgunReloadCycleLogicAnimNotifyState::NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *,
    const FAnimNotifyEventReference &EventReference)
{
    ABBBShotgunEquipment *Equipment = MeshComp ? Cast<ABBBShotgunEquipment>(MeshComp->GetOwner()) : nullptr;
    if (!Equipment || !Equipment->IsEquipped() || Equipment->IsMirror())
    {
        return;
    }

    if (UAnimNotifyLibrary::NotifyStateReachedEnd(EventReference))
    {
        Equipment->SubmitInput(FBBBShotgunReloadCycleEndLocalControlPacket{bFinishing});
        return;
    }

    Equipment->SubmitInput(FBBBShotgunInterruptReloadLocalControlPacket{});
}

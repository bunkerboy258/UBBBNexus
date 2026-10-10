#include "BBBWork/UBBBNexus/Notify/Equipment/Logic/BBBShotgunReloadCycleLogicAnimNotifyState.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/BBBShotgunEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/LocalControl/Reload/FBBBShotgunReloadCycleEndLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/LocalControl/Reload/FBBBShotgunInterruptReloadLocalControlPacket.h"
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
        ensureMsgf(Equipment->SubmitInput(FBBBShotgunReloadCycleEndLocalControlPacket{}),
            TEXT("霰弹枪装填周期结束通知提交失败 %s"), *Equipment->GetName());
        return;
    }

    ensureMsgf(Equipment->SubmitInput(FBBBShotgunInterruptReloadLocalControlPacket{}),
        TEXT("霰弹枪装填取消通知提交失败 %s"), *Equipment->GetName());
}

#include "BBBWork/UBBBNexus/Notify/Equipment/Logic/BBBRifleReloadLifecycleLogicAnimNotifyState.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Reload/FBBBRifleInterruptReloadLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBRifleReloadLifecycleLogicAnimNotifyState::NotifyEnd(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    const FAnimNotifyEventReference &)
{
    ABBBRifleEquipment *Rifle = MeshComp ? Cast<ABBBRifleEquipment>(MeshComp->GetOwner()) : nullptr;
    if (!Rifle || !Rifle->IsEquipped() || Rifle->IsMirror())
    {
        return;
    }

    // 生命周期通知只报告动画结束 不在通知回调中直接修改换弹玩法状态
    if (!ensureMsgf(Rifle->SubmitInput(FBBBRifleInterruptReloadLocalControlPacket{}),
        TEXT("武器换弹收束通知提交输入失败 %s"), *Rifle->GetName()))
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] Reload lifecycle input submitted Equipment=%s"), *Rifle->GetName());
}

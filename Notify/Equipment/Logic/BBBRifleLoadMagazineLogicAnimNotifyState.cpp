#include "BBBWork/UBBBNexus/Notify/Equipment/Logic/BBBRifleLoadMagazineLogicAnimNotifyState.h"

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Reload/FBBBRifleLoadMagazineLocalControlPacket.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBRifleLoadMagazineLogicAnimNotifyState::NotifyBegin(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    float,
    const FAnimNotifyEventReference &)
{
    ABBBRifleEquipment *Rifle = MeshComp ? Cast<ABBBRifleEquipment>(MeshComp->GetOwner()) : nullptr;
    if (!Rifle || !Rifle->IsEquipped() || Rifle->IsMirror())
    {
        return;
    }

    // 网络镜像不得由本机动画生成装填事实 输入只进入步枪自身的解析管线
    if (!ensureMsgf(Rifle->SubmitInput(FBBBRifleLoadMagazineLocalControlPacket{}),
        TEXT("武器装填通知提交输入失败 %s"), *Rifle->GetName()))
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[BBBRifleMagazine] Load input submitted Equipment=%s"), *Rifle->GetName());
}

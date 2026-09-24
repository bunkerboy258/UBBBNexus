#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Notify/BBBRifleReloadLifecycleAnimNotifyState.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBRifleReloadLifecycleAnimNotifyState::NotifyEnd(
    USkeletalMeshComponent *MeshComp,
    UAnimSequenceBase *,
    const FAnimNotifyEventReference &)
{
    // 通知从动画网格所属角色读取当前激活步枪 保持事件目标与角色装备持有关系一致
    ABBBCharacter *Character = MeshComp ? Cast<ABBBCharacter>(MeshComp->GetOwner()) : nullptr;
    ABBBRifleEquipment *Rifle = Character ? Cast<ABBBRifleEquipment>(Character->GetActiveEquipment()) : nullptr;

    // 当前装备无效或镜像执行路径不允许从本机动画生成结果
    if (!Rifle || Rifle->IsMirror())
    {
        return;
    }

    // 通知状态区间结束时只提交中断输入 不在动画回调中判定换弹结果
    Rifle->SubmitInterruptReloadInput();
}

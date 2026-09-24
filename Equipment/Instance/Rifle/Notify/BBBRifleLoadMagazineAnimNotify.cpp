#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Notify/BBBRifleLoadMagazineAnimNotify.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/BBBRifleEquipment.h"
#include "Components/SkeletalMeshComponent.h"

void UBBBRifleLoadMagazineAnimNotify::Notify(
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

    // 这里只提交装入输入 弹药状态由步枪解析流程处理
    Rifle->SubmitLoadMagazineInput();
}

#include "BBBWork/UBBBNexus/Equipment/Template/Animation/BBBEquipmentAnimInstance.h"

#include "Animation/AnimMontage.h"

bool UBBBEquipmentAnimInstance::PlayEquipmentMontage(UAnimMontage &Montage)
{
    if (!ensureMsgf(IsInGameThread(), TEXT("装备蒙太奇只能在游戏线程开始播放")))
    {
        return false;
    }

    const float Duration = Montage_Play(&Montage);
    return ensureMsgf(
        Duration > 0.0f,
        TEXT("装备动画实例无法播放蒙太奇 %s"),
        *Montage.GetName());
}

void UBBBEquipmentAnimInstance::PublishAnimationFacts(const FBBBEquipmentAnimationFacts &Facts)
{
    // 保存装备动画事实供装备动画图读取
    AnimationFacts = Facts;
}

#include "BBBWork/UBBBNexus/Equipment/Template/Animation/BBBEquipmentAnimInstance.h"

void UBBBEquipmentAnimInstance::PublishAnimationFacts(const FBBBEquipmentAnimationFacts &Facts)
{
    // 保存装备动画事实供装备动画图读取
    AnimationFacts = Facts;
}

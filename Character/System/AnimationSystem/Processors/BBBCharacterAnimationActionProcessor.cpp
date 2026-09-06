#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/Processors/BBBCharacterAnimationActionProcessor.h"

#include "BBBWork/UBBBNexus/Character/System/AnimationSystem/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Events/BBBCharacterEquipmentEvents.h"

void FBBBCharacterAnimationActionProcessor::Update(
    UBBBAnimInstance &AnimInstance,
    const FBBBCharacterEquipmentEvents &EquipmentEvents) const
{
    // 读取装备系统本帧已经通过角色动作仲裁的瞬时动作
    const TArray<FBBBEquipmentActionEvent> &ActionEvents = EquipmentEvents.GetActionEvents();

    // 一个装备事件对应一次角色动作发布，不在动画处理器内合并或丢弃事件
    for (const FBBBEquipmentActionEvent &Event : ActionEvents)
    {
        // 蒙太奇由产生动作的武器领域填充，角色动画系统只负责转发
        if (!ensureMsgf(Event.Presentation.Montage, TEXT("[UBBBC]Equipment action presentation has no montage")))
        {
            continue;
        }

        AnimInstance.SubmitEquipmentActionMontage(Event);
    }
}

#include "BBBWork/UBBBNexus/Character/Instance/System/AnimationSystem/Processors/BBBCharacterAnimationLayerProcessor.h"

#include "BBBWork/UBBBNexus/Character/Instance/Core/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Controller/EquipmentController/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/BBBEquipment.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterAnimationLayerProcessor::Update(
    const FBBBCharacterEquipmentState &EquipmentState,
    const FBBBCharacterAnimationConfig &AnimationConfig,
    FBBBAnimationRuntimeData &AnimationData,
    USkeletalMeshComponent &CharacterMesh) const
{
    // 默认使用角色动画层并允许当前装备覆盖
    TSubclassOf<UAnimInstance> DesiredLayerClass = AnimationConfig.DefaultAnimationLayerClass;
    ABBBEquipment *ActiveInstance = EquipmentState.GetActiveMainHandInstance();

    if (ActiveInstance)
    {
        // 已装备武器时优先使用武器指定的角色动画层
        const TSubclassOf<UAnimInstance> EquipmentLayerClass = ActiveInstance->GetCharacterAnimationLayerClass();
        if (EquipmentLayerClass)
        {
            DesiredLayerClass = EquipmentLayerClass;
        }
    }

    if (!ensureMsgf(
        DesiredLayerClass,
        TEXT("[UBBBC]No animation layer is configured for the character or active equipment")))
    {
        return;
    }

    // 动画层未变化时避免重复重连
    if (AnimationData.LinkedAnimationLayerClass == DesiredLayerClass)
    {
        return;
    }

    // 重连动画层后保存当前类用于下一帧比较
    CharacterMesh.LinkAnimClassLayers(DesiredLayerClass);
    AnimationData.LinkedAnimationLayerClass = DesiredLayerClass;
}

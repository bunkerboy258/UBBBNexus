#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/Processors/BBBCharacterAnimationLayerProcessor.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/States/BBBCharacterAnimationLayerState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipment.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterAnimationLayerProcessor::Update(
    FBBBCharacterAnimationUpdateContext &Context) const
{
    const FBBBCharacterEquipmentSelectionState &EquipmentState = Context.EquipmentSelectionState;
    const FBBBCharacterAnimationConfig &AnimationConfig = Context.AnimationConfig;
    FBBBCharacterAnimationLayerState &LayerState = Context.AnimationLayerState;
    USkeletalMeshComponent &CharacterMesh = Context.CharacterMesh;
    // 默认使用角色动画层并允许当前装备覆盖
    TSubclassOf<UAnimInstance> DesiredLayerClass = AnimationConfig.DefaultAnimationLayerClass;
    ABBBEquipment *ActiveInstance = EquipmentState.ActiveMainHandInstance;

    if (ActiveInstance)
    {
        // 已装备武器时优先使用武器指定的角色动画层
        const TSubclassOf<UAnimInstance> EquipmentLayerClass = ActiveInstance->GetCharacterAnimationLayerClass();
        if (EquipmentLayerClass)
        {
            DesiredLayerClass = EquipmentLayerClass;
        }
    }

    if (!DesiredLayerClass)
    {
        return;
    }

    // 动画层未变化时避免重复重连
    if (LayerState.LinkedAnimationLayerClass == DesiredLayerClass)
    {
        return;
    }

    // 重连动画层后保存当前类用于下一帧比较
    CharacterMesh.LinkAnimClassLayers(DesiredLayerClass);
    LayerState.LinkedAnimationLayerClass = DesiredLayerClass;
}

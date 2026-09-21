#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/BBBCharacterAnimationSystem.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/State/BBBAnimationState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/LocomotionController/Definition/BBBCharacterControlState.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterAnimationSystem::Initialize(
    ABBBCharacter &InCharacter,
    FBBBCharacterRuntimeData &InRuntimeData,
    USkeletalMeshComponent &InCharacterMesh,
    FBBBAnimationState &InAnimationData,
    const FBBBCharacterEquipmentState &InEquipmentState,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBCharacterAnimationConfig &InAnimationConfig)
{
    Character = &InCharacter;
    RuntimeData = &InRuntimeData;
    CharacterMesh = &InCharacterMesh;
    AnimationData = &InAnimationData;
    EquipmentState = &InEquipmentState;
    WorldData = &InWorldData;
    AnimationConfig = &InAnimationConfig;
}

//------------------------------------------------------------------------------

void FBBBCharacterAnimationSystem::Update()
{
    // 动画更新需要角色运行数据装备状态网格和动画配置全部有效
    if (!(AnimationData
            && Character
            && RuntimeData
            && EquipmentState
            && CharacterMesh
            && WorldData
            && AnimationConfig))
    {
        return;
    }

    // 动画系统必须使用角色动画实例才能接收事实和动作
    UBBBAnimInstance *AnimInstance = Cast<UBBBAnimInstance>(CharacterMesh->GetAnimInstance());
    if (!AnimInstance)
    {
        return;
    }

    // 先根据装备状态更新动画层
    LayerProcessor.Update(
        *EquipmentState,
        *AnimationConfig,
        *AnimationData,
        *CharacterMesh);
    // 再处理换弹和装备动作动画
    ActionProcessor.Update(
        *AnimInstance,
        *AnimationData);

    // 在移动完成后采集角色动画事实
    FactProcessor.Update(
        *Character,
        *RuntimeData,
        AnimationData->Facts,
        WorldData->FrameDeltaSeconds);
    // 将最终事实发布到主动画实例
    AnimInstance->PublishAnimationFacts(AnimationData->Facts);
}

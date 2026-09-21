#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/BBBCharacterAnimationSystem.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/AnimationInstance/BBBAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterAnimationSystem::Initialize(
    ABBBCharacter &InCharacter,
    FBBBCharacterRuntimeData &InRuntimeData,
    USkeletalMeshComponent &InCharacterMesh,
    const FBBBCharacterAnimationConfig &InAnimationConfig)
{
    Character = &InCharacter;
    RuntimeData = &InRuntimeData;
    CharacterMesh = &InCharacterMesh;
    AnimationConfig = &InAnimationConfig;
}

//------------------------------------------------------------------------------

void FBBBCharacterAnimationSystem::Update()
{
    // 动画更新需要角色运行数据装备状态网格和动画配置全部有效
    if (!(Character
            && RuntimeData
            && CharacterMesh
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

    FBBBCharacterAnimationUpdateContext Context{
        *Character,
        *RuntimeData,
        *CharacterMesh,
        *AnimInstance,
        RuntimeData->Animation.AnimationFactState,
        RuntimeData->Animation.AnimationLayerState,
        RuntimeData->Equipment.ReadEquipmentSelectionState(),
        RuntimeData->External.ReadWorldState(),
        *AnimationConfig};

    LayerProcessor.Update(Context);
    ActionProcessor.Update(Context);
    FactProcessor.Update(Context);
    AnimInstance->PublishAnimationFacts(Context.AnimationFactState);
}

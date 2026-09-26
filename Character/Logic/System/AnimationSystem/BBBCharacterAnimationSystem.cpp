#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/BBBCharacterAnimationSystem.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/Context/BBBCharacterAnimationUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Animation/BBBAnimInstance.h"
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
        ensureMsgf(false, TEXT("角色动画系统缺少 BBB 动画实例 蒙太奇请求将在动画实例有效后处理"));
        return;
    }

    FBBBCharacterAnimationUpdateContext Context{
        *Character,
        *RuntimeData,
        *CharacterMesh,
        *AnimInstance,
        RuntimeData->Animation.AnimationFactState,
        RuntimeData->Animation.AnimationLayerState,
        RuntimeData->Animation.AnimationMontageState,
        RuntimeData->Equipment.ReadEquipmentSelectionState(),
        RuntimeData->External.ReadWorldState(),
        *AnimationConfig};

    LayerProcessor.Update(Context);
    MontageProcessor.Update(Context);
    ActionProcessor.Update(Context);
    FactProcessor.Update(Context);
    MuzzleProcessor.Update(Context);
    AnimInstance->PublishAnimationFacts(Context.AnimationFactState);
}

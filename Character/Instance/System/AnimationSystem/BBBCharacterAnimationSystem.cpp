#include "BBBWork/UBBBNexus/Character/Instance/System/AnimationSystem/BBBCharacterAnimationSystem.h"

#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Instance/Core/Config/Animation/BBBCharacterAnimationConfig.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/BBBAnimInstance.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/AnimationSystem/Definition/BBBAnimationRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Character/Instance/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"

void FBBBCharacterAnimationSystem::Initialize(
    ABBBCharacter &InCharacter,
    FBBBCharacterRuntimeData &InRuntimeData,
    USkeletalMeshComponent &InCharacterMesh,
    FBBBAnimationRuntimeData &InAnimationData,
    const FBBBCharacterEquipmentState &InEquipmentState,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBCharacterAnimationConfig &InAnimationConfig)
{
    Character = &InCharacter;
    RuntimeData = &InRuntimeData;
    CharacterMesh = &InCharacterMesh;
    AnimationData = &InAnimationData;
    EquipmentState = &InEquipmentState;
    WorldData = &InWorldData;
    IntentData = &InIntentData;
    AnimationConfig = &InAnimationConfig;
}

//------------------------------------------------------------------------------

void FBBBCharacterAnimationSystem::Update()
{
    // 动画更新需要角色运行数据装备状态网格和动画配置全部有效
    if (!ensureMsgf(
        AnimationData
            && Character
            && RuntimeData
            && EquipmentState
            && CharacterMesh
            && WorldData
            && IntentData
            && AnimationConfig,
        TEXT("[UBBBC]Animation system update failed because dependencies are null")))
    {
        return;
    }

    // 动画系统必须使用角色动画实例才能接收事实和动作
    UBBBAnimInstance *AnimInstance = Cast<UBBBAnimInstance>(CharacterMesh->GetAnimInstance());
    if (!ensureMsgf(
        AnimInstance,
        TEXT("[UBBBC]Character mesh must use UBBBAnimInstance or a derived animation blueprint")))
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

    if (IntentData->WantsDash())
    {
        // 根据移动输入选择对应方向的冲刺动画
        const FVector2D MoveInput = IntentData->GetMoveInput();
        UAnimMontage *DashMontage = AnimationConfig->DashForwardMontage;

        if (FMath::Abs(MoveInput.X) > FMath::Abs(MoveInput.Y))
        {
            DashMontage = MoveInput.X < 0.0f
                ? AnimationConfig->DashLeftMontage
                : AnimationConfig->DashRightMontage;
        }

        if (MoveInput.Y < -0.5f)
        {
            DashMontage = AnimationConfig->DashBackwardMontage;
        }

        AnimInstance->PlayMovementActionMontage(DashMontage);
    }

    if (IntentData->WantsSlide())
    {
        // 滑铲意图触发一次滑铲动画
        AnimInstance->PlayMovementActionMontage(AnimationConfig->SlideMontage);
    }
    // 在移动完成后采集角色动画事实
    FactProcessor.Update(
        *Character,
        *RuntimeData,
        AnimationData->Facts,
        WorldData->GetFrameDeltaSeconds());
    // 将最终事实发布到主动画实例
    AnimInstance->PublishAnimationFacts(AnimationData->Facts);
}

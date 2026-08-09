#include "BBBWork/UBBBNexus/Character/System/FacingSystem/BBBCharacterFacingSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Facing/BBBFacingConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/States/BBBAimStates.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/BBBFacingRuntimeData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"

void FBBBCharacterFacingSystem::Initialize(
    APawn &InPawn,
    UCharacterMovementComponent &InMovement,
    FBBBFacingRuntimeData &InFacingData,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBAimRuntimeData &InAimData,
    const FBBBCharacterFacingConfig &InConfig)
{
    Pawn = &InPawn;
    Movement = &InMovement;
    FacingData = &InFacingData;
    WorldData = &InWorldData;
    IntentData = &InIntentData;
    AimData = &InAimData;
    Config = &InConfig;
}

//------------------------------------------------------------------------------

void FBBBCharacterFacingSystem::Update()
{
    if (!ensureMsgf(
        Pawn && Movement && FacingData && WorldData && IntentData && AimData && Config,
        TEXT("[UBBBC]Facing system update failed because dependencies are null")))
    {
        return;
    }

    const FBBBAimRuntimeState &AimState = AimData->GetState();
    const bool bUseCustomFacing = AimState.bIsAiming || IntentData->WantsFire();

    //没有瞄准或开火时恢复角色移动组件的正常朝向逻辑
    if (!bUseCustomFacing)
    {
        Movement->bOrientRotationToMovement = true;
        Movement->bUseControllerDesiredRotation = false;

        FacingData->CommitState(false);
        return;
    }

    //瞄准或开火时停用移动朝向并启用自定义区间转向
    Movement->bOrientRotationToMovement = false;
    Movement->bUseControllerDesiredRotation = false;

    const FVector ToTarget = FVector(AimState.AimTargetWorld) - Pawn->GetActorLocation();
    const FVector HorizontalToTarget(ToTarget.X, ToTarget.Y, 0.0f);

    //目标与起点重合时没有可计算的朝向
    if (HorizontalToTarget.IsNearlyZero())
    {
        FacingData->CommitState(false);
        return;
    }

    //计算角色当前朝向与目标方向之间的水平偏角
    const float TargetYaw = HorizontalToTarget.Rotation().Yaw;
    const float CurrentAimYawAbs = FMath::Abs(
        FMath::FindDeltaAngleDegrees(Pawn->GetActorRotation().Yaw, TargetYaw));
    const bool bWasBodyTurning = FacingData->IsBodyTurning();

    //启动与停止使用不同阈值，避免临界角度反复转向
    if (!bWasBodyTurning && CurrentAimYawAbs <= Config->MaxAimYawBeforeBodyTurn)
    {
        FacingData->CommitState(false);
        return;
    }

    if (bWasBodyTurning && CurrentAimYawAbs <= Config->AimYawBodyTurnStopThreshold)
    {
        FacingData->CommitState(false);
        return;
    }

    const FRotator TargetRotation(0.0f, TargetYaw, 0.0f);
    const FRotator UpdatedRotation = FMath::RInterpTo(
        Pawn->GetActorRotation(),
        TargetRotation,
        WorldData->GetFrameDeltaSeconds(),
        Config->ArmedBodyTurnInterpSpeed);

    //只修改角色的水平朝向
    Pawn->SetActorRotation(UpdatedRotation);
    FacingData->CommitState(true);
}

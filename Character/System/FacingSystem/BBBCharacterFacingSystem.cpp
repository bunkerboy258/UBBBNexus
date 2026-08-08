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
    const bool bIsAiming = AimState.bIsAiming;
    const bool bIsFiring = IntentData->WantsFire();
    const bool bShouldFaceAimDirection = bIsAiming || bIsFiring;

    //瞄准或开火时锁定身体朝向，其他情况面向移动方向
    Movement->bOrientRotationToMovement = !bShouldFaceAimDirection;
    Movement->bUseControllerDesiredRotation = false;

    if (!bShouldFaceAimDirection || !Pawn->GetController())
    {
        FacingData->CommitState(false, 0.0f);
        return;
    }

    const float CurrentAimYawAbs = FMath::Abs(AimState.AimYaw);
    const bool bWasBodyTurning = FacingData->IsBodyTurning();

    //身体尚未转向且偏角未超过启动阈值时保持不动
    if (!bWasBodyTurning && CurrentAimYawAbs <= Config->MaxAimYawBeforeBodyTurn)
    {
        FacingData->CommitState(false, CurrentAimYawAbs);
        return;
    }

    //身体开始转向后必须追到停止阈值内才结束
    if (bWasBodyTurning && CurrentAimYawAbs <= Config->AimYawBodyTurnStopThreshold)
    {
        FacingData->CommitState(false, CurrentAimYawAbs);
        return;
    }

    //两个不同阈值构成滞回，避免临界角度反复启停
    //计算瞄准来源指向目标的世界方向
    const FVector ToTarget = FVector(AimState.AimTargetWorld) - AimData->GetAimOriginWorld();

    //目标与来源重合时无法得到有效朝向
    if (ToTarget.IsNearlyZero())
    {
        FacingData->CommitState(false, CurrentAimYawAbs);
        return;
    }

    //只提取目标方向的水平旋转
    const FRotator TargetRotation(0.0f, ToTarget.Rotation().Yaw, 0.0f);

    //按照配置速度逐帧追赶目标方向
    const FRotator UpdatedRotation = FMath::RInterpTo(
        Pawn->GetActorRotation(),
        TargetRotation,
        WorldData->GetFrameDeltaSeconds(),
        Config->ArmedBodyTurnInterpSpeed);

    //应用本帧水平旋转并记录身体正在转向
    Pawn->SetActorRotation(UpdatedRotation);
    FacingData->CommitState(true, CurrentAimYawAbs);
}

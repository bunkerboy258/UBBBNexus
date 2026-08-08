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

    //尚未开始转向时允许上半身在阈值内独立瞄准
    if (!bWasBodyTurning && CurrentAimYawAbs <= Config->MaxAimYawBeforeBodyTurn)
    {
        FacingData->CommitState(false, CurrentAimYawAbs);
        return;
    }

    //身体追赶至停止阈值后结束转向，形成稳定滞回区间
    if (bWasBodyTurning && CurrentAimYawAbs <= Config->AimYawBodyTurnStopThreshold)
    {
        FacingData->CommitState(false, CurrentAimYawAbs);
        return;
    }

    const FVector ToTarget = FVector(AimState.AimTargetWorld) - AimData->GetAimOriginWorld();
    if (ToTarget.IsNearlyZero())
    {
        FacingData->CommitState(false, CurrentAimYawAbs);
        return;
    }

    const FRotator TargetRotation(0.0f, ToTarget.Rotation().Yaw, 0.0f);
    const FRotator UpdatedRotation = FMath::RInterpTo(
        Pawn->GetActorRotation(),
        TargetRotation,
        WorldData->GetFrameDeltaSeconds(),
        Config->ArmedBodyTurnInterpSpeed);

    //只旋转角色水平朝向，不修改俯仰与横滚
    Pawn->SetActorRotation(UpdatedRotation);
    FacingData->CommitState(true, CurrentAimYawAbs);
}

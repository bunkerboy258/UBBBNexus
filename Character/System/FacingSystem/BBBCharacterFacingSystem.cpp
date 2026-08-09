#include "BBBWork/UBBBNexus/Character/System/FacingSystem/BBBCharacterFacingSystem.h"

#include "BBBWork/UBBBNexus/Character/Core/Config/Facing/BBBFacingConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Definition/BBBCharacterWorldRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/States/BBBAimStates.h"
#include "BBBWork/UBBBNexus/Character/System/FacingSystem/Definition/BBBFacingRuntimeData.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"

void FBBBCharacterFacingSystem::Initialize(
    APawn &InPawn,
    UCharacterMovementComponent &InMovement,
    USkeletalMeshComponent &InCharacterMesh,
    FBBBFacingRuntimeData &InFacingData,
    const FBBBCharacterWorldRuntimeData &InWorldData,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBAimRuntimeData &InAimData,
    const FBBBCharacterFacingConfig &InConfig)
{
    Pawn = &InPawn;
    Movement = &InMovement;
    CharacterMesh = &InCharacterMesh;
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
        Pawn && Movement && CharacterMesh && FacingData && WorldData && IntentData && AimData && Config,
        TEXT("[UBBBC]Facing system update failed because dependencies are null")))
    {
        return;
    }

    //骨骼配置无效时保留可用的角色空间回退点
    FVector AimOriginWorld = Pawn->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
    if (ensureMsgf(
        !Config->AimOriginBoneName.IsNone()
            && CharacterMesh->DoesSocketExist(Config->AimOriginBoneName),
        TEXT("[UBBBC]Facing aim origin bone is not configured or missing on character mesh")))
    {
        AimOriginWorld = CharacterMesh->GetSocketLocation(Config->AimOriginBoneName);
    }

    const FBBBAimRuntimeState &AimState = AimData->GetState();
    const bool bUseCustomFacing = AimState.bIsAiming || IntentData->WantsFire();

    //没有瞄准或开火时恢复角色移动组件的正常朝向逻辑
    if (!bUseCustomFacing)
    {
        if (Pawn->IsLocallyControlled())
        {
            Movement->bOrientRotationToMovement = true;
            Movement->bUseControllerDesiredRotation = false;
        }

        FacingData->CommitState(false, 0.0f, AimOriginWorld);
        return;
    }

    //瞄准或开火时停用移动朝向并启用自定义区间转向
    if (Pawn->IsLocallyControlled())
    {
        Movement->bOrientRotationToMovement = false;
        Movement->bUseControllerDesiredRotation = false;
    }

    const FVector ToTarget = FVector(AimState.AimTargetWorld) - AimOriginWorld;

    //目标与起点重合时没有可计算的朝向
    if (ToTarget.IsNearlyZero())
    {
        FacingData->CommitState(false, 0.0f, AimOriginWorld);
        return;
    }

    //把世界瞄准方向转换为角色自身的水平偏角
    const FMatrix ReferenceMatrix = FRotationMatrix::MakeFromXZ(
        Pawn->GetActorForwardVector(),
        Pawn->GetActorUpVector());
    const FVector LocalAimDirection = ReferenceMatrix.InverseTransformVector(ToTarget.GetSafeNormal());
    const float TargetAimYaw = FMath::Clamp(LocalAimDirection.Rotation().Yaw, -90.0f, 90.0f);
    const float AimYaw = FMath::FInterpTo(
        FacingData->GetAimYaw(),
        TargetAimYaw,
        WorldData->GetFrameDeltaSeconds(),
        Config->AimYawInterpSpeed);

    //远端角色只重建表现事实，不覆盖引擎同步的角色旋转
    if (!Pawn->IsLocallyControlled())
    {
        FacingData->CommitState(false, AimYaw, AimOriginWorld);
        return;
    }

    if (!Pawn->GetController())
    {
        FacingData->CommitState(false, AimYaw, AimOriginWorld);
        return;
    }

    const float CurrentAimYawAbs = FMath::Abs(AimYaw);
    const bool bWasBodyTurning = FacingData->IsBodyTurning();

    //启动与停止使用不同阈值，避免临界角度反复转向
    if (!bWasBodyTurning && CurrentAimYawAbs <= Config->MaxAimYawBeforeBodyTurn)
    {
        FacingData->CommitState(false, AimYaw, AimOriginWorld);
        return;
    }

    if (bWasBodyTurning && CurrentAimYawAbs <= Config->AimYawBodyTurnStopThreshold)
    {
        FacingData->CommitState(false, AimYaw, AimOriginWorld);
        return;
    }

    const FRotator TargetRotation(0.0f, ToTarget.Rotation().Yaw, 0.0f);
    const FRotator UpdatedRotation = FMath::RInterpTo(
        Pawn->GetActorRotation(),
        TargetRotation,
        WorldData->GetFrameDeltaSeconds(),
        Config->ArmedBodyTurnInterpSpeed);

    //只修改本地角色的水平朝向
    Pawn->SetActorRotation(UpdatedRotation);
    FacingData->CommitState(true, AimYaw, AimOriginWorld);
}

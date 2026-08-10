
#include "BBBWork/UBBBNexus/Character/System/LocomotionSystem/BBBCharacterLocomotionSystem.h"
#include "BBBWork/UBBBNexus/Character/Core/Config/Locomotion/BBBLocomotionConfig.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Intent/Definition/BBBIntentRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/AimSystem/Definition/BBBAimRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterLocomotionSystem::Initialize(
    ACharacter &InPawn,
    UCharacterMovementComponent &InMovement,
    const FBBBIntentRuntimeData &InIntentData,
    const FBBBAimRuntimeData &InAimData,
    const FBBBCharacterEquipmentState &InEquipmentState,
    const FBBBCharacterLocomotionConfig &InConfig)
{
    Character = &InPawn;
    Movement = &InMovement;
    IntentData = &InIntentData;
    AimData = &InAimData;
    EquipmentState = &InEquipmentState;
    Config = &InConfig;
}

void FBBBCharacterLocomotionSystem::Update()
{
    if (!ensureMsgf(Character && Movement && IntentData && AimData && EquipmentState && Config, TEXT("[UBBBC]Locomotion system update failed because dependencies are null")))
    {
        return;
    }

    const bool bWantsCrouch = IntentData->WantsCrouch();

    //按住蹲伏时请求引擎进入蹲伏
    if (bWantsCrouch)
    {
        Character->Crouch();
    }

    //松开蹲伏时请求引擎在空间允许后起身
    if (!bWantsCrouch)
    {
        Character->UnCrouch();
    }

    const bool bUsesCrouchMovement = bWantsCrouch || Movement->IsCrouching();

    const bool bHasMainHandEquipment = EquipmentState->GetActiveMainHandInstance() != nullptr;
    const bool bIsAiming = AimData->GetState().bIsAiming;
    const FBBBCharacterLocomotionProfileConfig *Profile = &Config->Unarmed;
    const FBBBCharacterLocomotionProfileConfig *CrouchProfile = &Config->CrouchUnarmed;

    //持有主手装备时使用装备移动参数
    if (bHasMainHandEquipment)
    {
        Profile = &Config->MainHandEquipped;
        CrouchProfile = &Config->CrouchMainHandEquipped;
    }

    //实际进入瞄准状态后使用侧向移动参数
    if (bIsAiming)
    {
        Profile = &Config->Strafe;
        CrouchProfile = &Config->CrouchStrafe;
    }

    //请求或保持蹲伏时切换到相同装备与瞄准语义下的蹲伏配置
    if (bUsesCrouchMovement)
    {
        Profile = CrouchProfile;
    }

    float DesiredSpeed = Profile->WalkSpeed;
    float DesiredAcceleration = Profile->WalkAcceleration;

    //冲刺意图只负责选择当前姿态下的跑步参数
    if (IntentData->WantsSprint())
    {
        DesiredSpeed = Profile->RunSpeed;
        DesiredAcceleration = Profile->RunAcceleration;
    }

    const float ClampedDesiredSpeed = FMath::Max(DesiredSpeed, 1.0f);

    Movement->MaxWalkSpeed = ClampedDesiredSpeed;
    Movement->MaxWalkSpeedCrouched = ClampedDesiredSpeed;
    Movement->MaxAcceleration = FMath::Max(DesiredAcceleration, 0.0f);

    //跳跃由移动系统提交给角色移动组件
    if (IntentData->WantsJump())
    {
        Character->Jump();
    }

    if (!IntentData->HasMoveInput())
    {
        return;
    }

    const FRotator YawRotation(0.0f, Character->GetControlRotation().Yaw, 0.0f);

    //将局部输入转换为相机水平朝向下的世界移动方向
    Character->AddMovementInput(
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X),
        IntentData->GetMoveInput().Y);

    Character->AddMovementInput(
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y),
        IntentData->GetMoveInput().X);
}

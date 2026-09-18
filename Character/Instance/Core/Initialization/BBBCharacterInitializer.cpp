
#include "BBBWork/UBBBNexus/Character/Instance/Core/Initialization/BBBCharacterInitializer.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Instance/Runtime/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void FBBBCharacterInitializer::Initialize(ABBBCharacter &Character)
{
    const FBBBCharacterConfig &Config = Character.CharacterConfig;

    // 初始化开始先确认网络组件和配置资源有效
    //网络组件存在？
    if (!ensureMsgf(Character.CharacterNetworkComponent, TEXT("[UBBBC]Character initialization failed: CharacterNetworkComponent is null")))
    { return; }

    if (!ensureMsgf(
        Config.Equipment.EquipmentCatalog,
        TEXT("[UBBBC]Character '%s' of class '%s' has no CharacterConfig.Equipment.EquipmentCatalog"),
        *Character.GetName(),
        *Character.GetClass()->GetPathName()))
    { return; }
    
    UCharacterMovementComponent *Movement = Character.GetCharacterMovement();

    //相机骨骼网格与移动组件存在？
    if (!ensureMsgf(Character.GetMesh() && Movement, TEXT("[UBBBC]Character initialization failed because engine components are null")))
    { return; }

    //确保角色黑板更新完成后再启动骨骼动画更新
    Character.GetMesh()->AddTickPrerequisiteComponent(Movement);

    Character.Input.Initialize(Character.RuntimeData.Input);
    FBBBCharacterControlInput InitialControl;
    InitialControl.FacingWorld = Character.GetActorRotation();
    Character.Input.Submit(InitialControl);
    Character.bUseControllerRotationYaw = false;
    
    // 按固定顺序注入各角色系统和运行数据
    Character.AimSystem.Initialize(
        Character.RuntimeData.Aim,
        Character.RuntimeData.Control);

    Character.LocomotionSystem.Initialize(
        Character,
        *Movement,
        Character.RuntimeData.Locomotion,
        Character.RuntimeData.Control,
        Config.Locomotion);
    
    Character.EquipmentSystem.Initialize(
        *Character.GetMesh(),
        Character.RuntimeData.Equipment,
        Character,
        Config.Equipment);
    
    Character.NetworkSystem.Initialize(
        Character.RuntimeData.Network,
        Character.RuntimeData.Aim,
        Character.RuntimeData.Locomotion,
        Character.RuntimeData.Equipment.Equipment,
        *Character.CharacterNetworkComponent,
        *Config.Equipment.EquipmentCatalog,
        Character.RuntimeData.WorldData,
        Character.Input,
        Character.RuntimeData.Equipment.Events,
        Config.Network);

    Character.CharacterNetworkComponent->Initialize(
        Character.NetworkSystem);
    
    Character.AnimationSystem.Initialize(
        Character,
        Character.RuntimeData,
        *Character.GetMesh(),
        Character.RuntimeData.Animation,
        Character.RuntimeData.Equipment.Equipment,
        Character.RuntimeData.WorldData,
        Config.Animation);
    
    Character.InputPipeline.Data = &Character.RuntimeData;
    Character.ArbitrationPipeline.Data = &Character.RuntimeData;
    Character.CharacterUpdatePipeline.Initialize(Character);

    Character.GetCapsuleComponent()->SetCapsuleSize(
        FMath::Max(Config.Locomotion.CapsuleRadius, 1.0f),
        FMath::Max(Config.Locomotion.CapsuleHalfHeight, 1.0f));

    Movement->MaxWalkSpeed = FMath::Max(Config.Locomotion.RunSpeeds.X, 1.0f);
    Movement->MaxWalkSpeedCrouched = FMath::Max(Config.Locomotion.CrouchSpeeds.X, 1.0f);
    Movement->MinAnalogWalkSpeed = FMath::Max(Config.Locomotion.MinAnalogWalkSpeed, 0.0f);
    Movement->MaxAcceleration = FMath::Max(Config.Locomotion.MaxAcceleration, 0.0f);
    Movement->BrakingDecelerationWalking = FMath::Max(
        Config.Locomotion.BrakingDeceleration,
        0.0f);
    Movement->GroundFriction = FMath::Max(Config.Locomotion.GroundFriction, 0.0f);
    Movement->BrakingFriction = FMath::Max(Config.Locomotion.BrakingFriction, 0.0f);
    Movement->BrakingFrictionFactor = FMath::Max(
        Config.Locomotion.BrakingFrictionFactor,
        0.0f);
    Movement->bUseSeparateBrakingFriction = false;
    Movement->BrakingSubStepTime = Config.Locomotion.BrakingSubStepTime;
    Movement->AirControl = Config.Locomotion.AirControl;
    Movement->GravityScale = Config.Locomotion.GravityScale;
    Movement->MaxStepHeight = Config.Locomotion.MaxStepHeight;
    Movement->SetWalkableFloorAngle(Config.Locomotion.WalkableFloorAngle);
    Movement->JumpZVelocity = Config.Locomotion.JumpZVelocity;
    Movement->bOrientRotationToMovement = false;
    Movement->bUseControllerDesiredRotation = false;
    Movement->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

    Movement->GetNavAgentPropertiesRef().bCanCrouch = true;
    Movement->SetCrouchedHalfHeight(FMath::Max(Config.Locomotion.CrouchedHalfHeight, 1.0f));
}


#include "BBBWork/UBBBNexus/Character/Core/Initialization/BBBCharacterInitializer.h"
#include "Camera/CameraComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/States/BBBInputRawData.h"
#include "BBBWork/UBBBNexus/Character/Runtime/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"

void FBBBCharacterInitializer::Initialize(ABBBCharacter &Character)
{
    const FBBBCharacterConfig &Config = Character.CharacterConfig;

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
    if (!ensureMsgf(Character.CameraBoom && Character.GetMesh() && Movement, TEXT("[UBBBC]Character initialization failed because engine components are null")))
    { return; }

    //确保角色黑板更新完成后再启动骨骼动画更新
    Character.GetMesh()->AddTickPrerequisiteActor(&Character);
    
    Character.CharacterExternalAPI.Initialize(
        Character.RuntimeData.Animation.Commands,
        Character.RuntimeData.Camera.Commands);
    
    Character.CameraSystem.Initialize(
        Character,
        *Character.CameraBoom,
        Character.RuntimeData.Camera,
        Character.RuntimeData.WorldData,
        Character.RuntimeData.Input,
        Character.RuntimeData.Intent,
        Config.Camera);
    
    Character.AimSystem.Initialize(
        Character,
        *Character.GetMesh(),
        Character.RuntimeData.Aim,
        Character.RuntimeData.WorldData,
        Character.RuntimeData.Intent,
        Character.RuntimeData.Equipment.Equipment,
        Character.RuntimeData.Animation.GetCommands(),
        Config.Aim,
        Config.AimAnimation);
    
    Character.LocomotionSystem.Initialize(
        Character,
        *Movement,
        Character.RuntimeData.Intent,
        Character.RuntimeData.Aim,
        Character.RuntimeData.Equipment.Equipment,
        Config.Locomotion);
    
    Character.FacingSystem.Initialize(
        Character,
        *Movement,
        Character.RuntimeData.Facing,
        Character.RuntimeData.WorldData,
        Character.RuntimeData.Intent,
        Character.RuntimeData.Aim,
        Config.Facing);
    
    Character.EquipmentSystem.Initialize(
        *Character.GetMesh(),
        Character.RuntimeData.Equipment,
        Character.CharacterExternalAPI,
        Character,
        Config.Equipment);
    
    Character.CharacterNetworkComponent->Initialize(
        Character.RuntimeData.Network);

    Character.NetworkSystem.Initialize(
        Character.RuntimeData.Network,
        Character.RuntimeData.Aim,
        Character.RuntimeData.Equipment.Equipment,
        *Character.CharacterNetworkComponent,
        *Config.Equipment.EquipmentCatalog,
        Character.RuntimeData.WorldData,
        Character.RuntimeData.Equipment.Commands,
        Character.RuntimeData.Equipment.Results,
        Config.Aim);
    
    Character.AnimationSystem.Initialize(
        *Character.GetMesh(),
        *Movement,
        Character.RuntimeData.Animation,
        Character.RuntimeData.AnimationState,
        Character.RuntimeData.WorldData,
        Character.RuntimeData.Aim,
        Character.RuntimeData.Facing,
        Character.RuntimeData.Equipment.Equipment,
        Config.Aim,
        Config.AimAnimation,
        Config.Equipment.AimSourceBoneName);
    
    Character.InputPipeline.Initialize(
        Character.RuntimeData.Input,
        Character.RuntimeData.WorldData,
        Character.RuntimeData.Input.RawInputData,
        Config.Input.Pipeline);
    
    Character.IntentPipeline.Initialize(
        Character.RuntimeData.Intent,
        Character.RuntimeData.Input);
    
    Character.RequestPipeline.Initialize(
        Character.RuntimeData.Decision,
        Character.RuntimeData.Intent);
    
    Character.ArbitrationPipeline.Initialize(
        Character.RuntimeData.Decision,
        Character.RuntimeData.Equipment.Equipment);
    
    Character.ExecutionPipeline.Initialize(
        Character.RuntimeData.Decision,
        Character.RuntimeData.Equipment.Commands,
        Character.RuntimeData.Equipment.Equipment,
        Character.RuntimeData.Equipment.Inventory);
    
    Character.CharacterUpdatePipeline.Initialize(
        Character,
        Character.RuntimeData,
        Character.CameraSystem,
        Character.AimSystem,
        Character.LocomotionSystem,
        Character.FacingSystem,
        Character.EquipmentSystem,
        Character.NetworkSystem,
        Character.AnimationSystem,
        Character.InputPipeline,
        Character.IntentPipeline,
        Character.RequestPipeline,
        Character.ArbitrationPipeline,
        Character.ExecutionPipeline);

    //把相机配置应用到构造阶段创建的弹簧臂
    if (Character.CameraBoom)
    {
        //设置相机与角色之间的基础距离
        Character.CameraBoom->TargetArmLength = Config.Camera.CameraBoomLength;

        //设置弹簧臂末端相机插槽偏移
        Character.CameraBoom->SocketOffset = Config.Camera.CameraBoomSocketOffset;

        //设置弹簧臂相对角色根组件的目标偏移
        Character.CameraBoom->TargetOffset = Config.Camera.CameraBoomTargetOffset;

        //让弹簧臂跟随控制器视角旋转
        Character.CameraBoom->bUsePawnControlRotation = true;

        //按配置启用相机位置滞后
        Character.CameraBoom->bEnableCameraLag = Config.Camera.bCameraLag;

        //设置相机滞后追赶速度
        Character.CameraBoom->CameraLagSpeed = Config.Camera.CameraLagSpeed;
    }

    //把相机配置应用到弹簧臂末端的实际相机
    if (Character.FollowCamera)
    {
        //避免相机与弹簧臂同时应用控制器旋转
        Character.FollowCamera->bUsePawnControlRotation = false;

        //设置相机相对弹簧臂插槽的局部偏移
        Character.FollowCamera->SetRelativeLocation(Config.Camera.CameraRelativeLocation);
    }

    //使用最小正值防止非法配置彻底冻结移动组件
    Movement->MaxWalkSpeed = FMath::Max(Config.Locomotion.Unarmed.RunSpeed, 1.0f);

    //将角色加速度配置同步到引擎移动组件
    Movement->MaxAcceleration = FMath::Max(Config.Locomotion.Unarmed.RunAcceleration, 0.0f);

    //将角色起跳速度配置同步到引擎移动组件
    Movement->JumpZVelocity = Config.Locomotion.JumpZVelocity;
}

void FBBBCharacterInitializer::BindInput(ABBBCharacter &Character, UInputComponent *PlayerInputComponent)
{
    //项目只支持增强输入组件 拒绝普通输入组件
    UEnhancedInputComponent *Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);

    //类型不匹配时 不建立任何不完整绑定
    if (!Input)
    { return; }
    
    const FBBBCharacterInputConfig &Config = Character.CharacterConfig.Input;
    
    if (Config.MoveAction)
    {
        Input->BindActionValueLambda(
            Config.MoveAction,
            ETriggerEvent::Triggered,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.SetMoveAxis(Value.Get<FVector2D>());
            });
        
        Input->BindActionValueLambda(
            Config.MoveAction,
            ETriggerEvent::Completed,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.SetMoveAxis(FVector2D::ZeroVector);
            });
        
        Input->BindActionValueLambda(
            Config.MoveAction,
            ETriggerEvent::Canceled,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.SetMoveAxis(FVector2D::ZeroVector);
            });
    }
    
    if (Config.LookAction)
    {
        Input->BindActionValueLambda(
            Config.LookAction,
            ETriggerEvent::Triggered,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.SetLookAxis(Value.Get<FVector2D>());
            });
    }
    
    if (Config.FireAction)
    {
        Input->BindActionValueLambda(
            Config.FireAction,
            ETriggerEvent::Started,
            [&Character](const FInputActionValue &Value)
            {
                //设置连续开火状态为1
                Character.RuntimeData.Input.RawInputData.SetFireHeld(true);

                //触发开火开始的边沿事件
                Character.RuntimeData.Input.RawInputData.MarkFireStarted();
            });
        
        Input->BindActionValueLambda(
            Config.FireAction,
            ETriggerEvent::Completed,
            [&Character](const FInputActionValue &Value)
            {
                //设置连续开火状态为0
                Character.RuntimeData.Input.RawInputData.SetFireHeld(false);

                //触发开火结束的边沿事件
                Character.RuntimeData.Input.RawInputData.MarkFireCompleted();
            });

        //输入被取消时 执行与正常结束相同的状态收束
        Input->BindActionValueLambda(
            Config.FireAction,
            ETriggerEvent::Canceled,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.SetFireHeld(false);

                Character.RuntimeData.Input.RawInputData.MarkFireCompleted();
            });
    }
    
    if (Config.ReloadAction)
    {
        Input->BindActionValueLambda(
            Config.ReloadAction,
            ETriggerEvent::Started,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.MarkReloadPressed();
            });
    }
    
    if (Config.EquipSlot1Action)
    {
        Input->BindActionValueLambda(
            Config.EquipSlot1Action,
            ETriggerEvent::Started,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.MarkEquipSlot1Pressed();
            });
    }
    
    if (Config.EquipSlot2Action)
    {
        Input->BindActionValueLambda(
            Config.EquipSlot2Action,
            ETriggerEvent::Started,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.MarkEquipSlot2Pressed();
            });
    }
    
    //瞄准逻辑与开火同理
    if (Config.PrecisionAimAction)
    {
        Input->BindActionValueLambda(
            Config.PrecisionAimAction,
            ETriggerEvent::Started,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.SetPrecisionAimHeld(true);
                
                Character.RuntimeData.Input.RawInputData.MarkPrecisionAimStarted();
            });
        
        Input->BindActionValueLambda(
            Config.PrecisionAimAction,
            ETriggerEvent::Completed,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.SetPrecisionAimHeld(false);
                
                Character.RuntimeData.Input.RawInputData.MarkPrecisionAimCompleted();
            });
        
        Input->BindActionValueLambda(
            Config.PrecisionAimAction,
            ETriggerEvent::Canceled,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.SetPrecisionAimHeld(false);
                
                Character.RuntimeData.Input.RawInputData.MarkPrecisionAimCompleted();
            });
    }
    
    if (Config.SprintAction)
    {
        Input->BindActionValueLambda(
            Config.SprintAction,
            ETriggerEvent::Started,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.SetSprintHeld(true);
            });
        
        Input->BindActionValueLambda(
            Config.SprintAction,
            ETriggerEvent::Completed,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.SetSprintHeld(false);
            });
        
        Input->BindActionValueLambda(
            Config.SprintAction,
            ETriggerEvent::Canceled,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.SetSprintHeld(false);
            });
    }

    if (Config.JumpAction)
    {
        Input->BindActionValueLambda(
            Config.JumpAction,
            ETriggerEvent::Started,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.Input.RawInputData.MarkJumpPressed();
            });
    }
}

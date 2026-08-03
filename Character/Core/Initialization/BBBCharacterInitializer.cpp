
#include "BBBWork/UBBBNexus/Character/Core/Initialization/BBBCharacterInitializer.h"
#include "Camera/CameraComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Pipeline/Input/Definition/BBBInputRawData.h"
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

    if (!ensureMsgf(Config.Item.EquipmentCatalog, TEXT("[UBBBC]Character initialization failed because equipment catalog is null")))
    { return; }
    
    UCharacterMovementComponent *Movement = Character.GetCharacterMovement();

    //相机骨骼网格与移动组件存在？
    if (!ensureMsgf(Character.CameraBoom && Character.GetMesh() && Movement, TEXT("[UBBBC]Character initialization failed because engine components are null")))
    { return; }
    
    Character.CharacterExternalAPI.Initialize(
        Character.RuntimeData.CharacterData.Animation.Commands,
        Character.RuntimeData.CharacterData.Camera.Commands);
    
    Character.CameraSystem.Initialize(
        Character,
        *Character.CameraBoom,
        Character.RuntimeData.CharacterData.Camera,
        Character.RuntimeData.WorldData,
        Character.RuntimeData.CharacterData.Input,
        Character.RuntimeData.CharacterData.Intent,
        Config.Camera);
    
    Character.AimSystem.Initialize(
        Character,
        *Character.GetMesh(),
        Character.RuntimeData.CharacterData.Aim,
        Character.RuntimeData.WorldData,
        Character.RuntimeData.CharacterData.Intent,
        Character.RuntimeData.CharacterData.Item.State.Equipment,
        Character.RuntimeData.CharacterData.Animation.GetCommands(),
        Config.Aim,
        Config.AimAnimation);
    
    Character.LocomotionSystem.Initialize(
        Character,
        *Movement,
        Character.RuntimeData.CharacterData.Intent,
        Config.Locomotion);
    
    Character.FacingSystem.Initialize(
        Character,
        *Movement,
        Character.RuntimeData.CharacterData.Facing,
        Character.RuntimeData.WorldData,
        Character.RuntimeData.CharacterData.Intent,
        Character.RuntimeData.CharacterData.Aim,
        Config.Facing);
    
    Character.ItemSystem.Initialize(
        *Character.GetMesh(),
        Character.RuntimeData.CharacterData.Item,
        Character.CharacterExternalAPI,
        Character,
        Config.Equipment,
        Config.Item,
        Character.RuntimeData.WorldData);
    
    Character.CharacterNetworkComponent->Initialize(
        Character.RuntimeData.CharacterData.Network);

    Character.NetworkSystem.Initialize(
        Character.RuntimeData.CharacterData.Network,
        Character.RuntimeData.CharacterData.Aim,
        Character.RuntimeData.CharacterData.Item.State.Equipment,
        *Character.CharacterNetworkComponent,
        *Config.Item.EquipmentCatalog,
        Character.RuntimeData.WorldData,
        Character.RuntimeData.CharacterData.Item.Commands,
        Character.RuntimeData.CharacterData.Item.ActionResults,
        Config.Aim);
    
    Character.AnimationSystem.Initialize(
        *Character.GetMesh(),
        *Movement,
        Character.RuntimeData.CharacterData.Animation,
        Character.RuntimeData.CharacterPresentationData.AnimationState,
        Character.RuntimeData.WorldData,
        Character.RuntimeData.CharacterData.Aim,
        Character.RuntimeData.CharacterData.Facing,
        Character.RuntimeData.CharacterData.Intent,
        Character.RuntimeData.CharacterData.Item.State.Equipment,
        Config.Aim,
        Config.AimAnimation,
        Config.Locomotion,
        Config.Equipment.AimSourceBoneName);
    
    Character.InputPipeline.Initialize(
        Character.RuntimeData.CharacterData.Input,
        Character.RuntimeData.WorldData,
        Character.RuntimeData.InputRawData,
        Config.Input.Pipeline);
    
    Character.IntentPipeline.Initialize(
        Character.RuntimeData.CharacterData.Intent,
        Character.RuntimeData.WorldData,
        Character.RuntimeData.CharacterData.Input,
        Config.Locomotion);
    
    Character.RequestPipeline.Initialize(
        Character.RuntimeData.CharacterData.Decision,
        Character.RuntimeData.CharacterData.Intent);
    
    Character.ArbitrationPipeline.Initialize(
        Character.RuntimeData.CharacterData.Decision,
        Character.RuntimeData.CharacterData.Item.State.Equipment);
    
    Character.ExecutionPipeline.Initialize(
        Character.RuntimeData.CharacterData.Decision,
        Character.RuntimeData.CharacterData.Item.Commands,
        Character.RuntimeData.CharacterData.Item.State.Equipment,
        Character.RuntimeData.CharacterData.Item.State.Inventory);
    
    Character.CharacterUpdatePipeline.Initialize(
        Character,
        Character.RuntimeData,
        Character.CameraSystem,
        Character.AimSystem,
        Character.LocomotionSystem,
        Character.FacingSystem,
        Character.ItemSystem,
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
    Movement->MaxWalkSpeed = FMath::Max(Config.Locomotion.RunSpeed, 1.0f);

    //将角色加速度配置同步到引擎移动组件
    Movement->MaxAcceleration = Config.Locomotion.RunAcceleration;
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
                Character.RuntimeData.InputRawData.SetMoveAxis(Value.Get<FVector2D>());
            });
        
        Input->BindActionValueLambda(
            Config.MoveAction,
            ETriggerEvent::Completed,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.InputRawData.SetMoveAxis(FVector2D::ZeroVector);
            });
        
        Input->BindActionValueLambda(
            Config.MoveAction,
            ETriggerEvent::Canceled,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.InputRawData.SetMoveAxis(FVector2D::ZeroVector);
            });
    }
    
    if (Config.LookAction)
    {
        Input->BindActionValueLambda(
            Config.LookAction,
            ETriggerEvent::Triggered,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.InputRawData.SetLookAxis(Value.Get<FVector2D>());
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
                Character.RuntimeData.InputRawData.SetFireHeld(true);

                //触发开火开始的边沿事件
                Character.RuntimeData.InputRawData.MarkFireStarted();
            });
        
        Input->BindActionValueLambda(
            Config.FireAction,
            ETriggerEvent::Completed,
            [&Character](const FInputActionValue &Value)
            {
                //设置连续开火状态为0
                Character.RuntimeData.InputRawData.SetFireHeld(false);

                //触发开火结束的边沿事件
                Character.RuntimeData.InputRawData.MarkFireCompleted();
            });

        //输入被取消时 执行与正常结束相同的状态收束
        Input->BindActionValueLambda(
            Config.FireAction,
            ETriggerEvent::Canceled,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.InputRawData.SetFireHeld(false);

                Character.RuntimeData.InputRawData.MarkFireCompleted();
            });
    }
    
    if (Config.ReloadAction)
    {
        Input->BindActionValueLambda(
            Config.ReloadAction,
            ETriggerEvent::Started,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.InputRawData.MarkReloadPressed();
            });
    }
    
    if (Config.EquipSlot1Action)
    {
        Input->BindActionValueLambda(
            Config.EquipSlot1Action,
            ETriggerEvent::Started,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.InputRawData.MarkEquipSlot1Pressed();
            });
    }
    
    if (Config.EquipSlot2Action)
    {
        Input->BindActionValueLambda(
            Config.EquipSlot2Action,
            ETriggerEvent::Started,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.InputRawData.MarkEquipSlot2Pressed();
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
                Character.RuntimeData.InputRawData.SetPrecisionAimHeld(true);
                
                Character.RuntimeData.InputRawData.MarkPrecisionAimStarted();
            });
        
        Input->BindActionValueLambda(
            Config.PrecisionAimAction,
            ETriggerEvent::Completed,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.InputRawData.SetPrecisionAimHeld(false);
                
                Character.RuntimeData.InputRawData.MarkPrecisionAimCompleted();
            });
        
        Input->BindActionValueLambda(
            Config.PrecisionAimAction,
            ETriggerEvent::Canceled,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.InputRawData.SetPrecisionAimHeld(false);
                
                Character.RuntimeData.InputRawData.MarkPrecisionAimCompleted();
            });
    }
    
    if (Config.SprintAction)
    {
        Input->BindActionValueLambda(
            Config.SprintAction,
            ETriggerEvent::Started,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.InputRawData.SetSprintHeld(true);
            });
        
        Input->BindActionValueLambda(
            Config.SprintAction,
            ETriggerEvent::Completed,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.InputRawData.SetSprintHeld(false);
            });
        
        Input->BindActionValueLambda(
            Config.SprintAction,
            ETriggerEvent::Canceled,
            [&Character](const FInputActionValue &Value)
            {
                Character.RuntimeData.InputRawData.SetSprintHeld(false);
            });
    }
}

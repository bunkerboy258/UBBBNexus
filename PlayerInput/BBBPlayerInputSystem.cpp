#include "BBBWork/UBBBNexus/PlayerInput/BBBPlayerInputSystem.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/Local/Action/BBBJumpPacket.h"
#include "BBBWork/UBBBNexus/PlayerCamera/BBBPlayerCameraSystem.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

UBBBPlayerInputSystem::UBBBPlayerInputSystem()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UBBBPlayerInputSystem::SetCharacter(ABBBCharacter *Target)
{
    APlayerController *Controller = Cast<APlayerController>(GetOwner());
    const bool bValidLocalTarget = !Target
        || (Controller
            && Controller->IsLocalController()
            && Controller->GetPawn() == Target
            && Target->IsLocallyControlled());

    if (!ensureMsgf(bValidLocalTarget,
        TEXT("[BBBInput]Player input rejected a character that is not the local controller pawn")))
    {
        Target = nullptr;
    }

    if (Character.Get() == Target && (Target || !Camera))
    {
        return;
    }
    if (ABBBCharacter *Previous = Character.Get())
    {
        // 解绑时释放持续输入 保留最后朝向避免无输入帧将角色转向世界零度
        FBBBCharacterMovementPacket ReleasedControl;
        ReleasedControl.FacingWorld = Previous->GetActorRotation();
        Previous->SubmitInput(ReleasedControl);
        Previous->RemoveTickPrerequisiteComponent(this);
    }
    if (Camera)
    {
        if (Controller && Controller->GetViewTarget() == Camera)
        {
            Controller->SetViewTarget(Controller);
        }
        Camera->Destroy();
        Camera = nullptr;
    }
    Character = Target;
    MovementState = FBBBCharacterMovementPacket();
    AimState = FBBBCharacterAimPacket();
    bFire = false;
    bJump = false;
    MoveAxis = FVector2D::ZeroVector;
    LookAxis = FVector2D::ZeroVector;
    if (!Target)
    {
        return;
    }
    // 控制器先采样输入 适配层再提交数据 角色随后更新 不依赖相机本帧跟随计算
    AddTickPrerequisiteActor(Controller);
    Target->AddTickPrerequisiteComponent(this);
    if (!ensureMsgf(CameraClass, TEXT("[BBBInput]玩家输入组件未配置相机蓝图类")))
    {
        return;
    }
    FActorSpawnParameters Parameters;
    Parameters.Owner = Controller;
    Camera = GetWorld()->SpawnActor<ABBBPlayerCameraSystem>(
        CameraClass, Target->GetActorTransform(), Parameters);
    if (ensureMsgf(Camera, TEXT("[BBBInput]Camera creation failed")))
    {
        Camera->Initialize(*Target, *Controller);
        Controller->SetViewTarget(Camera);
    }
}

void UBBBPlayerInputSystem::SetInputEnabled(const bool bEnabled)
{
    bInputEnabled = bEnabled;
    if (!bEnabled)
    {
        MovementState = FBBBCharacterMovementPacket();
        AimState = FBBBCharacterAimPacket();
        bFire = false;
        bJump = false;
        MoveAxis = FVector2D::ZeroVector;
        LookAxis = FVector2D::ZeroVector;
    }
}

void UBBBPlayerInputSystem::SubmitEquipSlot(const int32 Slot)
{
    if (!bInputEnabled || !Character.IsValid())
    {
        return;
    }
    Character->GetEquipmentSystem().RequestSlot(Slot);
}

void UBBBPlayerInputSystem::SubmitReload()
{
    if (!bInputEnabled || !Character.IsValid())
    {
        return;
    }
    if (ABBBEquipment *Equipment = Character->GetActiveEquipment())
    {
        Equipment->SubmitReloadInput();
    }
}

void UBBBPlayerInputSystem::Bind(UEnhancedInputComponent &Input)
{
    if (!ensureMsgf(Config.MoveAction && Config.LookAction,
        TEXT("[BBBInput]Configure input actions on the player input component")))
    {
        return;
    }
    Input.BindActionValueLambda(Config.MoveAction, ETriggerEvent::Triggered,
        [this](const FInputActionValue &Value)
        {
            if (bInputEnabled)
            {
                MoveAxis = Value.Get<FVector2D>();
            }
        });
    Input.BindActionValueLambda(Config.LookAction, ETriggerEvent::Triggered,
        [this](const FInputActionValue &Value)
        {
            if (bInputEnabled)
            {
                LookAxis += Value.Get<FVector2D>();
            }
        });
    for (const ETriggerEvent Event : {ETriggerEvent::Completed, ETriggerEvent::Canceled})
    {
        Input.BindActionValueLambda(Config.MoveAction, Event,
            [this](const FInputActionValue &Value)
            {
                MoveAxis = FVector2D::ZeroVector;
            });
    }
    if (Config.FireAction)
    {
        Input.BindActionValueLambda(Config.FireAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                bFire = bInputEnabled;
            });
        for (const ETriggerEvent Event : {ETriggerEvent::Completed, ETriggerEvent::Canceled})
        {
            Input.BindActionValueLambda(Config.FireAction, Event,
                [this](const FInputActionValue &Value)
                {
                    bFire = false;
                });
        }
    }
    if (Config.PrecisionAimAction)
    {
        Input.BindActionValueLambda(Config.PrecisionAimAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                AimState.bAim = bInputEnabled;
            });
        for (const ETriggerEvent Event : {ETriggerEvent::Completed, ETriggerEvent::Canceled})
        {
            Input.BindActionValueLambda(Config.PrecisionAimAction, Event,
                [this](const FInputActionValue &Value)
                {
                    AimState.bAim = false;
                });
        }
    }
    if (Config.WalkAction)
    {
        Input.BindActionValueLambda(Config.WalkAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                MovementState.bWalk = bInputEnabled;
            });
        for (const ETriggerEvent Event : {ETriggerEvent::Completed, ETriggerEvent::Canceled})
        {
            Input.BindActionValueLambda(Config.WalkAction, Event,
                [this](const FInputActionValue &Value)
                {
                    MovementState.bWalk = false;
                });
        }
    }
    if (Config.SprintAction)
    {
        Input.BindActionValueLambda(Config.SprintAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                MovementState.bSprint = bInputEnabled;
            });
        for (const ETriggerEvent Event : {ETriggerEvent::Completed, ETriggerEvent::Canceled})
        {
            Input.BindActionValueLambda(Config.SprintAction, Event,
                [this](const FInputActionValue &Value)
                {
                    MovementState.bSprint = false;
                });
        }
    }
    if (Config.CrouchAction)
    {
        Input.BindActionValueLambda(Config.CrouchAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                MovementState.bCrouch = bInputEnabled;
            });
        for (const ETriggerEvent Event : {ETriggerEvent::Completed, ETriggerEvent::Canceled})
        {
            Input.BindActionValueLambda(Config.CrouchAction, Event,
                [this](const FInputActionValue &Value)
                {
                    MovementState.bCrouch = false;
                });
        }
    }
    if (Config.JumpAction)
    {
        Input.BindActionValueLambda(Config.JumpAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                bJump = bInputEnabled;
            });
    }
    if (Config.ReloadAction)
    {
        Input.BindActionValueLambda(Config.ReloadAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                SubmitReload();
            });
    }
    if (Config.EquipSlot1Action)
    {
        Input.BindActionValueLambda(Config.EquipSlot1Action, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                SubmitEquipSlot(0);
            });
    }
    if (Config.EquipSlot2Action)
    {
        Input.BindActionValueLambda(Config.EquipSlot2Action, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                SubmitEquipSlot(1);
            });
    }
}

void UBBBPlayerInputSystem::TickComponent(const float DeltaTime, const ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    APlayerController *Controller = Cast<APlayerController>(GetOwner());
    if (!Controller || !Controller->IsLocalController())
    {
        return;
    }
    SetCharacter(Cast<ABBBCharacter>(Controller->GetPawn()));
    if (!Character.IsValid())
    {
        return;
    }
    FRotator Facing = Controller->GetControlRotation();
    Facing.Yaw += LookAxis.X * BaseTurnRate;
    Facing.Pitch = FMath::ClampAngle(Facing.Pitch + LookAxis.Y * BaseTurnRate, -89.0f, 89.0f);
    Controller->SetControlRotation(Facing);
    LookAxis = FVector2D::ZeroVector;
    const FVector2D Axis = MoveAxis.Size() > Config.MoveDeadZone ? MoveAxis : FVector2D::ZeroVector;
    const FRotator Yaw(0.0f, Facing.Yaw, 0.0f);
    MovementState.MoveWorld = (Yaw.Vector() * Axis.Y
        + FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y) * Axis.X).GetClampedToMaxSize(1.0f);
    MovementState.FacingWorld = Facing;
    FVector ViewLocation;
    FRotator ViewRotation;
    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    AimState.AimTargetWorld = ViewLocation + Facing.Vector() * FMath::Max(AimTargetDistance, 1.0f);
    FBBBCharacterMovementPacket Control = MovementState;
    Control.bSprint = Control.bSprint && !bFire;
    Character->SubmitInput(Control);
    FBBBCharacterAimPacket Aim = AimState;
    Aim.bAim = Aim.bAim || bFire;
    Character->SubmitInput(Aim);

    // 开火与跳跃以包的存在与否表达按下 未按下时不投递
    if (bFire)
    {
        if (ABBBEquipment *Equipment = Character->GetActiveEquipment())
        {
            Equipment->SubmitPrimaryInput();
        }
    }
    if (bJump)
    {
        Character->SubmitInput(FBBBJumpPacket{});
    }
    bJump = false;
}

void UBBBPlayerInputSystem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    SetCharacter(nullptr);
    Super::EndPlay(EndPlayReason);
}

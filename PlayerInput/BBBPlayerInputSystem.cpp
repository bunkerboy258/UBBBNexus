#include "BBBWork/UBBBNexus/PlayerInput/BBBPlayerInputSystem.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
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
    if (Character.Get() == Target && (Target || !Camera))
    {
        return;
    }
    if (ABBBCharacter *Previous = Character.Get())
    {
        // 解绑时释放持续输入 保留最后朝向避免无输入帧将角色转向世界零度
        FBBBCharacterControlInput Released;
        Released.FacingWorld = Previous->GetActorRotation();
        Previous->GetInput().Submit(Released);
        Previous->RemoveTickPrerequisiteComponent(this);
    }
    APlayerController *Controller = Cast<APlayerController>(GetOwner());
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
    State = FBBBCharacterControlInput();
    MoveAxis = FVector2D::ZeroVector;
    LookAxis = FVector2D::ZeroVector;
    if (!Target || !Controller || !Controller->IsLocalController())
    {
        return;
    }
    // 控制器先采样输入 适配层再提交数据 角色随后更新 不依赖相机本帧跟随计算
    AddTickPrerequisiteActor(Controller);
    Target->AddTickPrerequisiteComponent(this);
    FActorSpawnParameters Parameters;
    Parameters.Owner = Controller;
    Camera = GetWorld()->SpawnActor<ABBBPlayerCameraSystem>(
        ABBBPlayerCameraSystem::StaticClass(), Target->GetActorTransform(), Parameters);
    if (ensureMsgf(Camera, TEXT("[BBBInput]Camera creation failed")))
    {
        Camera->Initialize(*Target, *Controller, CameraConfig);
        Controller->SetViewTarget(Camera);
    }
}

void UBBBPlayerInputSystem::SetInputEnabled(const bool bEnabled)
{
    bInputEnabled = bEnabled;
    if (!bEnabled)
    {
        State = FBBBCharacterControlInput();
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
    FBBBCharacterEquipInput Packet;
    Packet.EquipSlot = Slot;
    Character->GetInput().Submit(Packet);
}

void UBBBPlayerInputSystem::SubmitReload()
{
    if (!bInputEnabled || !Character.IsValid())
    {
        return;
    }
    Character->GetInput().Submit(FBBBCharacterReloadInput());
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
                State.bFire = bInputEnabled;
            });
        for (const ETriggerEvent Event : {ETriggerEvent::Completed, ETriggerEvent::Canceled})
        {
            Input.BindActionValueLambda(Config.FireAction, Event,
                [this](const FInputActionValue &Value)
                {
                    State.bFire = false;
                });
        }
    }
    if (Config.PrecisionAimAction)
    {
        Input.BindActionValueLambda(Config.PrecisionAimAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                State.bAim = bInputEnabled;
            });
        for (const ETriggerEvent Event : {ETriggerEvent::Completed, ETriggerEvent::Canceled})
        {
            Input.BindActionValueLambda(Config.PrecisionAimAction, Event,
                [this](const FInputActionValue &Value)
                {
                    State.bAim = false;
                });
        }
    }
    if (Config.WalkAction)
    {
        Input.BindActionValueLambda(Config.WalkAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                State.bWalk = bInputEnabled;
            });
        for (const ETriggerEvent Event : {ETriggerEvent::Completed, ETriggerEvent::Canceled})
        {
            Input.BindActionValueLambda(Config.WalkAction, Event,
                [this](const FInputActionValue &Value)
                {
                    State.bWalk = false;
                });
        }
    }
    if (Config.SprintAction)
    {
        Input.BindActionValueLambda(Config.SprintAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                State.bSprint = bInputEnabled;
            });
        for (const ETriggerEvent Event : {ETriggerEvent::Completed, ETriggerEvent::Canceled})
        {
            Input.BindActionValueLambda(Config.SprintAction, Event,
                [this](const FInputActionValue &Value)
                {
                    State.bSprint = false;
                });
        }
    }
    if (Config.CrouchAction)
    {
        Input.BindActionValueLambda(Config.CrouchAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                State.bCrouch = bInputEnabled;
            });
        for (const ETriggerEvent Event : {ETriggerEvent::Completed, ETriggerEvent::Canceled})
        {
            Input.BindActionValueLambda(Config.CrouchAction, Event,
                [this](const FInputActionValue &Value)
                {
                    State.bCrouch = false;
                });
        }
    }
    if (Config.JumpAction)
    {
        Input.BindActionValueLambda(Config.JumpAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                State.bJump = bInputEnabled;
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
    Facing.Yaw += LookAxis.X * CameraConfig.BaseTurnRate;
    Facing.Pitch = FMath::ClampAngle(Facing.Pitch + LookAxis.Y * CameraConfig.BaseTurnRate, -89.0f, 89.0f);
    Controller->SetControlRotation(Facing);
    LookAxis = FVector2D::ZeroVector;
    const FVector2D Axis = MoveAxis.Size() > Config.MoveDeadZone ? MoveAxis : FVector2D::ZeroVector;
    const FRotator Yaw(0.0f, Facing.Yaw, 0.0f);
    State.MoveWorld = (Yaw.Vector() * Axis.Y
        + FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y) * Axis.X).GetClampedToMaxSize(1.0f);
    State.FacingWorld = Facing;
    FVector ViewLocation;
    FRotator ViewRotation;
    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    State.AimTargetWorld = ViewLocation + Facing.Vector() * FMath::Max(AimTargetDistance, 1.0f);
    Character->GetInput().Submit(State);
    State.bJump = false;
}

void UBBBPlayerInputSystem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    SetCharacter(nullptr);
    Super::EndPlay(EndPlayReason);
}

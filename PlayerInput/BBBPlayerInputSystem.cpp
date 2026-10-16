#include "BBBWork/UBBBNexus/PlayerInput/BBBPlayerInputSystem.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Player/BBBPlayerController.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Locomotion/FBBBJumpLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Locomotion/FBBBRunLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Locomotion/FBBBCrouchLocalControlPacket.h"
#include "BBBWork/UBBBNexus/PlayerCamera/BBBPlayerCameraSystem.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Input/LocalControl/Equipment/FBBBEquipmentSecondaryLocalControlPacket.h"
#include "EnhancedInputComponent.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueBeginLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueCancelLocalControlPacket.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/BBBEquipment.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"

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
        FBBBCharacterMovementLocalControlPacket ReleasedControl;
        ReleasedControl.FacingWorld = Previous->GetActorRotation();
        Previous->SubmitInput(ReleasedControl);
        Previous->SubmitInput(FBBBCharacterRescueCancelLocalControlPacket{});
        SubmitRun(false);
        SubmitCrouch(false);
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
    MovementState = FBBBCharacterMovementLocalControlPacket();
    AimState = FBBBCharacterAimLocalControlPacket();
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
        if (ABBBCharacter *Target = Character.Get())
        {
            Target->SubmitInput(FBBBCharacterRescueCancelLocalControlPacket{});
        }
        SubmitRun(false);
        SubmitCrouch(false);
        MovementState = FBBBCharacterMovementLocalControlPacket();
        AimState = FBBBCharacterAimLocalControlPacket();
        bFire = false;
        bJump = false;
        MoveAxis = FVector2D::ZeroVector;
        LookAxis = FVector2D::ZeroVector;
    }
}

void UBBBPlayerInputSystem::SubmitItemSlot(const int32 Slot)
{
    if (!bInputEnabled || !Character.IsValid())
    {
        return;
    }
    if (Slot < 0 || Slot >= Character->RuntimeData.Item.ReadCharacterItemBarState().QuickAccessSlotCount)
    {
        return;
    }
    if (Character->RuntimeData.Traversal.ReadTraversalState().Action != EBBBTraversalAction::None)
    {
        return;
    }
    if (ABBBPlayerController *Controller = Cast<ABBBPlayerController>(GetOwner()))
    {
        int32 Start = 0;
        int32 Count = 0;
        if (Controller->IsConsumableBarHeld())
        {
            Controller->GetConsumableQuickRange(Start, Count);
        }
        Controller->SubmitItemSelect(Start + Slot);
    }
}

void UBBBPlayerInputSystem::SubmitReload()
{
    if (!bInputEnabled || !Character.IsValid())
    {
        return;
    }
    if (Character->RuntimeData.Traversal.ReadTraversalState().Action != EBBBTraversalAction::None)
    {
        return;
    }
    Character->SubmitInput(FBBBCharacterInteractionSecondaryLocalControlPacket{});
}

void UBBBPlayerInputSystem::SubmitRun(const bool bRun)
{
    if (ABBBCharacter *Target = Character.Get())
    {
        Target->SubmitInput(FBBBRunLocalControlPacket{bRun && bInputEnabled});
    }
}

void UBBBPlayerInputSystem::SubmitCrouch(const bool bCrouch)
{
    if (ABBBCharacter *Target = Character.Get())
    {
        Target->SubmitInput(FBBBCrouchLocalControlPacket{bCrouch && bInputEnabled});
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
                                        bFire = bInputEnabled && Character.IsValid() &&
                                                !Character->RuntimeData.Interaction.ReadInteractionUseState().bBusy;
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
                                        AimState.bAim =
                                            bInputEnabled && Character.IsValid() &&
                                            !Character->RuntimeData.Interaction.ReadInteractionUseState().bBusy;
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
    if (Config.RunAction)
    {
        Input.BindActionValueLambda(Config.RunAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                SubmitRun(true);
            });
        for (const ETriggerEvent Event : {ETriggerEvent::Completed, ETriggerEvent::Canceled})
        {
            Input.BindActionValueLambda(Config.RunAction, Event,
                [this](const FInputActionValue &Value)
                {
                    SubmitRun(false);
                });
        }
    }
    if (Config.CrouchAction)
    {
        Input.BindActionValueLambda(Config.CrouchAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                SubmitCrouch(true);
            });
        for (const ETriggerEvent Event : {ETriggerEvent::Completed, ETriggerEvent::Canceled})
        {
            Input.BindActionValueLambda(Config.CrouchAction, Event,
                [this](const FInputActionValue &Value)
                {
                    SubmitCrouch(false);
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
    if (Config.RescueAction)
    {
        Input.BindActionValueLambda(Config.RescueAction, ETriggerEvent::Started,
            [this](const FInputActionValue &Value)
            {
                if (bInputEnabled && Character.IsValid())
                {
                    Character->SubmitInput(FBBBCharacterRescueBeginLocalControlPacket{});
                }
            });
        for (const ETriggerEvent Event : {ETriggerEvent::Completed, ETriggerEvent::Canceled})
        {
            Input.BindActionValueLambda(Config.RescueAction, Event,
                [this](const FInputActionValue &Value)
                {
                    if (Character.IsValid())
                    {
                        Character->SubmitInput(FBBBCharacterRescueCancelLocalControlPacket{});
                    }
                });
        }
    }
    for (int32 Slot = 0; Slot < Config.ItemSlotActions.Num(); ++Slot)
    {
        if (!ensureMsgf(Config.ItemSlotActions[Slot], TEXT("[BBBInput]快捷槽位动作缺失 Slot=%d"), Slot))
        {
            continue;
        }
        Input.BindActionValueLambda(Config.ItemSlotActions[Slot], ETriggerEvent::Started,
            [this, Slot](const FInputActionValue &Value)
            {
                SubmitItemSlot(Slot);
            });
    }
}

FVector UBBBPlayerInputSystem::ResolveAimTargetWorld(const FVector& ViewLocation, const FVector& ViewDirection) const
{
    const FVector Direction = ViewDirection.GetSafeNormal();
    const float Distance = FMath::Max(AimTargetDistance, 1.0f);
    const FVector End = ViewLocation + Direction * Distance;
    UWorld* World = GetWorld();
    if (!World || Direction.IsNearlyZero())
    {
        return End;
    }

    FVector Start = ViewLocation;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBPlayerAimTarget), true);
    Params.AddIgnoredActor(GetOwner());
    Params.AddIgnoredActor(Camera);
    if (ABBBCharacter* Target = Character.Get())
    {
        Params.AddIgnoredActor(Target);
        Params.AddIgnoredActor(Target->GetHeldItem());
        // 第三人称相机后退段不生成角色身后的瞄准目标
        const double ForwardDistance = FMath::Clamp(
            FVector::DotProduct(Target->GetActorLocation() - ViewLocation, Direction), 0.0, Distance - 1.0);
        Start += Direction * ForwardDistance;
    }

    FHitResult WorldHit;
    const bool bWorldHit = World->LineTraceSingleByChannel(WorldHit, Start, End, ECC_Visibility, Params);
    FVector AimTarget = bWorldHit ? WorldHit.ImpactPoint : End;
    if (UBBBMassSubsystem* Mass = World->GetSubsystem<UBBBMassSubsystem>())
    {
        FMassEntityHandle HitEntity;
        float HitTime = 1.0f;
        FVector HitPosition;
        FVector HitNormal;
        EPhysicalSurface HitSurface;
        uint8 HitPart = 0;
        // 场景遮挡已裁短查询范围 不扫描墙后的实体
        if (Mass->TraceEntities(Start, AimTarget, 0.0f, {}, HitEntity, HitTime, HitPosition, HitNormal, HitSurface, HitPart))
        {
            AimTarget = HitPosition;
        }
    }

    // 只向角色提交当前目标 枪口姿态负责收敛 子弹仍从实际枪口正前方发射
    return AimTarget;
}

void UBBBPlayerInputSystem::TickComponent(const float DeltaTime, const ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction)
{
    if (Character.IsValid() && Character->RuntimeData.Interaction.ReadInteractionUseState().bBusy)
    {
        bFire = false;
        AimState.bAim = false;
    }

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
    AimState.AimTargetWorld = ResolveAimTargetWorld(ViewLocation, Facing.Vector());
    Character->SubmitInput(MovementState);
    Character->SubmitInput(AimState);

    // 开火与跳跃以包的存在与否表达按下 未按下时不投递
    if (bFire && Character->RuntimeData.Traversal.ReadTraversalState().Action == EBBBTraversalAction::None)
    {
        Character->SubmitInput(FBBBCharacterInteractionPrimaryLocalControlPacket{});
    }
    if (bJump)
    {
        Character->SubmitInput(FBBBJumpLocalControlPacket{});
    }
    bJump = false;
}

void UBBBPlayerInputSystem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    SetCharacter(nullptr);
    Super::EndPlay(EndPlayReason);
}

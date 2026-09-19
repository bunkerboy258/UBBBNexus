#include "BBBWork/UBBBNexus/PlayerCamera/BBBPlayerCameraSystem.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ABBBPlayerCameraSystem::ABBBPlayerCameraSystem()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PostUpdateWork;
    Boom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    SetRootComponent(Boom);
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(Boom, USpringArmComponent::SocketName);
    Boom->PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
    Boom->AddTickPrerequisiteActor(this);
}

void ABBBPlayerCameraSystem::Initialize(ABBBCharacter &InCharacter, APlayerController &InController,
    const FBBBPlayerCameraConfig &InConfig)
{
    Character = &InCharacter;
    Controller = &InController;
    Config = InConfig;
    AddTickPrerequisiteComponent(InCharacter.GetCharacterMovement());
    Boom->TargetArmLength = Config.CameraBoomLength;
    Boom->SocketOffset = Config.CameraBoomSocketOffset;
    Boom->TargetOffset = Config.CameraBoomTargetOffset;
    Boom->bEnableCameraLag = Config.bCameraLag;
    Boom->CameraLagSpeed = Config.CameraLagSpeed;
    Camera->SetRelativeLocation(Config.CameraRelativeLocation);
    SetActorLocationAndRotation(InCharacter.GetActorLocation(), InController.GetControlRotation());
}

void ABBBPlayerCameraSystem::Submit(const FBBBPlayerCameraInput &Packet)
{
    if (ensureMsgf(IsInGameThread() && !Packet.Impulse.ContainsNaN()
        && FMath::IsFinite(Packet.RecoverySpeed) && Packet.RecoverySpeed > 0.0f,
        TEXT("[BBBCamera]Invalid camera contribution")))
    {
        Pending.Add(Packet);
    }
}

void ABBBPlayerCameraSystem::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!Character.IsValid() || !Controller.IsValid())
    {
        return;
    }
    // 角色只发布提交方已经累计完成的最终相机输入
    if (Character->RuntimeData.CameraInput.IsSet())
    {
        Submit(Character->RuntimeData.CameraInput.GetValue());
        Character->RuntimeData.CameraInput.Reset();
    }
    FRotator Rotation = Controller->GetControlRotation();
    for (const FBBBPlayerCameraInput &Packet : Pending)
    {
        RecoilOffset += Packet.Impulse;
        Rotation.Pitch += Packet.Impulse.X;
        Rotation.Yaw += Packet.Impulse.Y;
        RecoverySpeed = Packet.RecoverySpeed;
    }
    Pending.Reset();
    const FVector2D NextOffset(
        FMath::FInterpTo(RecoilOffset.X, 0.0f, DeltaSeconds, RecoverySpeed),
        FMath::FInterpTo(RecoilOffset.Y, 0.0f, DeltaSeconds, RecoverySpeed));
    const FVector2D Delta = NextOffset - RecoilOffset;
    Rotation.Pitch += Delta.X;
    Rotation.Yaw += Delta.Y;
    RecoilOffset = NextOffset;
    Controller->SetControlRotation(Rotation);
    const bool bAiming = Character->RuntimeData.Control.WantsAim();
    Boom->TargetArmLength = FMath::FInterpTo(Boom->TargetArmLength,
        bAiming ? Config.AimBoomLength : Config.CameraBoomLength, DeltaSeconds, Config.AimBoomInterpSpeed);
    SetActorLocationAndRotation(Character->GetActorLocation(), Rotation);
}

#include "BBBWork/UBBBNexus/PlayerCamera/BBBPlayerCameraSystem.h"
#include "BBBWork/UBBBNexus/PlayerCamera/Processors/BBBPlayerCameraImpulseProcessor.h"
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

void ABBBPlayerCameraSystem::Initialize(ABBBCharacter &InCharacter, APlayerController &InController)
{
    Character = &InCharacter;
    Controller = &InController;
    AddTickPrerequisiteComponent(InCharacter.GetCharacterMovement());
    // 组件默认值由相机蓝图提供 运行期间只改变当前臂长
    DefaultBoomLength = Boom->TargetArmLength;
    ensureMsgf(FMath::IsFinite(DefaultBoomLength) && DefaultBoomLength > 0.0f
        && FMath::IsFinite(AimBoomLength) && AimBoomLength >= 0.0f
        && FMath::IsFinite(AimBoomInterpSpeed) && AimBoomInterpSpeed > 0.0f,
        TEXT("[BBBCamera]相机蓝图距离或插值速度无效"));
    SetActorLocationAndRotation(InCharacter.GetActorLocation(), InController.GetControlRotation());
}

void ABBBPlayerCameraSystem::Submit(const FBBBPlayerCameraInput &Packet)
{
    if (ensureMsgf(IsInGameThread() && !Packet.Impulse.ContainsNaN(),
        TEXT("[BBBCamera]Invalid camera contribution")))
    {
        Pending = Packet;
    }
}

void ABBBPlayerCameraSystem::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBPlayerCameraImpulseProcessor::Update(*this, DeltaSeconds);
}

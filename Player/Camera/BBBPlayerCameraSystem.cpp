#include "BBBWork/UBBBNexus/Player/Camera/BBBPlayerCameraSystem.h"
#include "BBBWork/UBBBNexus/Player/Camera/Processors/BBBPlayerCameraImpulseProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Camera/CameraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ABBBPlayerCameraSystem::ABBBPlayerCameraSystem()
{
    InnerOutlineMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(
        TEXT("/Game/_Project/Characters/BBBC_UA/Appearance/Materials/M_LocalCharacterInnerOutline.M_LocalCharacterInnerOutline")));
    PrimaryActorTick.bCanEverTick = true;
    // 步枪在 PostUpdateWork 发布快照 相机在该组全部完成后读取
    // NOTE: 相机缓存早于 PostUpdateWork 跟随必须在 PostPhysics 完成 武器贡献读取最近已发布快照
    PrimaryActorTick.TickGroup = TG_PostPhysics;
    PrimaryActorTick.EndTickGroup = TG_PostPhysics;
    Boom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    SetRootComponent(Boom);
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(Boom, USpringArmComponent::SocketName);
    Boom->PrimaryComponentTick.TickGroup = TG_PostPhysics;
    Boom->PrimaryComponentTick.EndTickGroup = TG_PostPhysics;
    Boom->AddTickPrerequisiteActor(this);
}

void ABBBPlayerCameraSystem::Initialize(ABBBCharacter &InCharacter, APlayerController &InController)
{
    if (InnerOutlineInstance)
    {
        Camera->PostProcessSettings.RemoveBlendable(InnerOutlineInstance.Get());
        InnerOutlineInstance = nullptr;
    }
    const auto &Readability = InCharacter.GetCharacterConfig().Appearance;
    if (InController.IsLocalController() && Readability.InnerOutlineOpacity > 0.0f)
    {
        UMaterialInterface *Parent = InnerOutlineMaterial.LoadSynchronous();
        if (ensureMsgf(Parent, TEXT("本地角色内侧轮廓材质缺失")))
        {
            InnerOutlineInstance = UMaterialInstanceDynamic::Create(Parent, this);
            InnerOutlineInstance->SetScalarParameterValue(TEXT("MaskStencil"), FMath::Clamp(Readability.ReadabilityStencil, 1, 255));
            InnerOutlineInstance->SetScalarParameterValue(TEXT("OutlineWidth"), FMath::Clamp(Readability.InnerOutlineWidth, 0.0f, 2.0f));
            InnerOutlineInstance->SetScalarParameterValue(TEXT("OutlineOpacity"), FMath::Clamp(Readability.InnerOutlineOpacity, 0.0f, 1.0f));
            Camera->PostProcessSettings.AddBlendable(InnerOutlineInstance.Get(), 1.0f);
        }
    }
    Character = &InCharacter;
    Controller = &InController;
    ActiveRecoilSettings = DefaultRecoilSettings;
    LastCameraRevision = InCharacter.RuntimeData.Parse.ReadCameraState().Revision;
    AddTickPrerequisiteComponent(InCharacter.GetCharacterMovement());
    FBBBCharacterUpdatePipeline::RegisterCameraTicks(InCharacter, *this);
    ensureMsgf(
        PrimaryActorTick.TickGroup == TG_PostPhysics
            && PrimaryActorTick.EndTickGroup == TG_PostPhysics
            && Boom->PrimaryComponentTick.TickGroup == TG_PostPhysics
            && Boom->PrimaryComponentTick.EndTickGroup == TG_PostPhysics,
        TEXT("[BBBCamera]相机与臂端必须在 PostPhysics 完成更新 否则相机缓存将落后一帧"));
    ensureMsgf(FMath::IsFinite(HipArmLength) && HipArmLength > 0.0f
        && FMath::IsFinite(AimArmLength) && AimArmLength > 0.0f && AimArmLength < HipArmLength
        && FMath::IsFinite(HipFieldOfView) && HipFieldOfView >= 30.0f && HipFieldOfView <= 120.0f
        && FMath::IsFinite(AimFieldOfView) && AimFieldOfView >= 30.0f && AimFieldOfView < HipFieldOfView
        && FMath::IsFinite(HipShoulderOffset) && HipShoulderOffset >= 0.0f
        && FMath::IsFinite(AimShoulderOffset) && AimShoulderOffset >= 0.0f
        && FMath::IsFinite(AimBoomInterpSpeed) && AimBoomInterpSpeed > 0.0f,
        TEXT("[BBBCamera]相机构图配置无效"));
    Boom->TargetArmLength = HipArmLength;
    Boom->TargetOffset.Y = 0.0f;
    Boom->SocketOffset.Y = HipShoulderOffset;
    Boom->bDoCollisionTest = true;
    FVector CameraRelativeLocation = Camera->GetRelativeLocation();
    CameraRelativeLocation.Y = 0.0f;
    Camera->SetRelativeLocation(CameraRelativeLocation);
    Camera->SetFieldOfView(HipFieldOfView);
    ensureMsgf(FMath::IsFinite(StanceHeightBlendDuration) && StanceHeightBlendDuration > 0.0f,
        TEXT("[BBBCamera]站蹲高度过渡时间必须为有限正数"));
    SmoothedCapsuleHalfHeight = InCharacter.GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    CapsuleHeightBlendStart = SmoothedCapsuleHalfHeight;
    CapsuleHeightBlendTarget = SmoothedCapsuleHalfHeight;
    CapsuleHeightBlendElapsed = 0.0f;
    SetActorLocationAndRotation(InCharacter.GetActorLocation(), InController.GetControlRotation());
}

void ABBBPlayerCameraSystem::Submit(const FBBBPlayerCameraInput &Packet)
{
    if (ensureMsgf(IsInGameThread() && !Packet.Impulse.ContainsNaN(),
        TEXT("[BBBCamera]Invalid camera contribution")))
    {
        if (!Pending.IsSet())
        {
            Pending = FBBBPlayerCameraInput{};
        }
        Pending->Impulse += Packet.Impulse;
    }
}

void ABBBPlayerCameraSystem::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FBBBPlayerCameraImpulseProcessor::Update(*this, DeltaSeconds);
}

void ABBBPlayerCameraSystem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (InnerOutlineInstance)
    {
        Camera->PostProcessSettings.RemoveBlendable(InnerOutlineInstance.Get());
        InnerOutlineInstance = nullptr;
    }
    Super::EndPlay(EndPlayReason);
}

#include "BBBWork/UBBBNexus/Player/Camera/BBBPlayerCameraSystem.h"
#include "BBBWork/UBBBNexus/Player/Camera/Processors/BBBPlayerCameraImpulseProcessor.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/Core/Update/BBBCharacterUpdatePipeline.h"
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
    CameraConfig = InCharacter.GetCharacterConfig().Camera;
    if (InController.IsLocalController())
    {
        // 游玩相机关闭画面四周的暗角
        Camera->PostProcessSettings.bOverride_VignetteIntensity = true;
        Camera->PostProcessSettings.VignetteIntensity = 0.0f;
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
    ensureMsgf(FMath::IsFinite(CameraConfig.HipArmLength) && CameraConfig.HipArmLength > 0.0f
        && FMath::IsFinite(CameraConfig.AimArmLength) && CameraConfig.AimArmLength > 0.0f
        && CameraConfig.AimArmLength < CameraConfig.HipArmLength
        && FMath::IsFinite(CameraConfig.HipFieldOfView) && CameraConfig.HipFieldOfView >= 30.0f
        && CameraConfig.HipFieldOfView <= 120.0f
        && FMath::IsFinite(CameraConfig.AimFieldOfView) && CameraConfig.AimFieldOfView >= 30.0f
        && CameraConfig.AimFieldOfView < CameraConfig.HipFieldOfView
        && FMath::IsFinite(CameraConfig.HipShoulderOffset) && CameraConfig.HipShoulderOffset >= 0.0f
        && FMath::IsFinite(CameraConfig.AimShoulderOffset) && CameraConfig.AimShoulderOffset >= 0.0f
        && FMath::IsFinite(CameraConfig.HipPivotHeight) && FMath::IsFinite(CameraConfig.AimPivotHeight)
        && FMath::IsFinite(CameraConfig.AimCrouchPivotDrop) && CameraConfig.AimCrouchPivotDrop >= 0.0f
        && FMath::IsFinite(CameraConfig.AimBlendDuration) && CameraConfig.AimBlendDuration > 0.0f
        && FMath::IsFinite(CameraConfig.AimFocusDistance) && CameraConfig.AimFocusDistance >= 100.0f
        && FMath::IsFinite(CameraConfig.AimDepthOfFieldFStop)
        && CameraConfig.AimDepthOfFieldFStop >= 1.0f && CameraConfig.AimDepthOfFieldFStop <= 32.0f
        && FMath::IsFinite(CameraConfig.RecoverySpeed) && CameraConfig.RecoverySpeed > 0.0f,
        TEXT("[BBBCamera]角色相机配置无效"));
    Boom->bDoCollisionTest = true;
    Camera->SetRelativeLocation(FVector::ZeroVector);
    AimBlendProgress = 0.0f;
    ApplyComposition(0.0f, 0.0f);

    ensureMsgf(FMath::IsFinite(CameraConfig.StanceHeightBlendDuration) && CameraConfig.StanceHeightBlendDuration > 0.0f,
        TEXT("[BBBCamera]站蹲高度过渡时间必须为有限正数"));
    SmoothedCapsuleHalfHeight = InCharacter.GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    CapsuleHeightBlendStart = SmoothedCapsuleHalfHeight;
    CapsuleHeightBlendTarget = SmoothedCapsuleHalfHeight;
    CapsuleHeightBlendElapsed = 0.0f;
    SetActorLocationAndRotation(InCharacter.GetActorLocation(), InController.GetControlRotation());
}

void ABBBPlayerCameraSystem::ApplyComposition(const float AimAlpha, const float CrouchAlpha)
{
    const float AimHeight = CameraConfig.AimPivotHeight - CameraConfig.AimCrouchPivotDrop * CrouchAlpha;
    Boom->TargetArmLength = FMath::Lerp(CameraConfig.HipArmLength, CameraConfig.AimArmLength, AimAlpha);
    Boom->TargetOffset = FVector(0.0f, 0.0f, FMath::Lerp(CameraConfig.HipPivotHeight, AimHeight, AimAlpha));
    Boom->SocketOffset = FVector(0.0f, FMath::Lerp(CameraConfig.HipShoulderOffset, CameraConfig.AimShoulderOffset, AimAlpha), 0.0f);
    Camera->SetFieldOfView(FMath::Lerp(CameraConfig.HipFieldOfView, CameraConfig.AimFieldOfView, AimAlpha));

    if (!Controller.IsValid() || !Controller->IsLocalController())
    {
        return;
    }

    const bool bApplyAimDepthOfField = AimAlpha > UE_KINDA_SMALL_NUMBER;
    FPostProcessSettings &PostProcess = Camera->PostProcessSettings;
    PostProcess.bOverride_DepthOfFieldEnabled = bApplyAimDepthOfField;
    PostProcess.bOverride_DepthOfFieldFocalDistance = bApplyAimDepthOfField;
    PostProcess.bOverride_DepthOfFieldFstop = bApplyAimDepthOfField;
    PostProcess.bOverride_DepthOfFieldDepthBlurRadius = bApplyAimDepthOfField;
    if (bApplyAimDepthOfField)
    {
        // 景深随镜头进度变化 对焦距离独立于准星射线的命中深度
        constexpr float ClosedFStop = 32.0f;
        PostProcess.DepthOfFieldEnabled = true;
        PostProcess.DepthOfFieldFocalDistance = CameraConfig.AimFocusDistance;
        PostProcess.DepthOfFieldFstop = 1.0f / FMath::Lerp(1.0f / ClosedFStop, 1.0f / CameraConfig.AimDepthOfFieldFStop, AimAlpha);
        PostProcess.DepthOfFieldDepthBlurRadius = 0.0f;
    }
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

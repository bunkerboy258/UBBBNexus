#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/WidgetComponent.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "PreviewScene.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/SceneComponent.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Animation/AnimSequence.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

void UBBBPlayerItemPortrait::SetActive(const bool bInActive)
{
    if (bActive == bInActive)
    {
        return;
    }
    bActive = bInActive;
    bDirty |= bActive;
    CaptureCredit = 0.0f;
    PendingPoseTime = 0.0f;
}

bool UBBBPlayerItemPortrait::IsForPawn(const APawn* Pawn) const
{
    return Source.IsValid() && Source.Get() == Pawn;
}

bool UBBBPlayerItemPortrait::Prepare(APawn *Pawn)
{
    if (Source.Get() != Pawn)
    {
        Close();
    }
    if (!IsValid(Pawn))
    {
        Close();
        return false;
    }
    Source = Pawn;
    UWorld *World = Pawn->GetWorld();
    if (!World || World->IsNetMode(NM_DedicatedServer))
    {
        return false;
    }
    if (IsValid(Display) && IsValid(Capture) && IsValid(Pose))
    {
        return true;
    }
    const FSoftObjectPath StandingPath(TEXT("/Game/_Project/Characters/Animation/OtherAnimation/Project/AS_Rifle_Idle_GunDown_SG550.AS_Rifle_Idle_GunDown_SG550"));
    const FSoftObjectPath RiflePath(TEXT("/Game/_Project/Characters/BBBC_UA/Items/Equipment/Rifle/SG550/DA_ModernWeapons_Rifle_02.DA_ModernWeapons_Rifle_02"));
    if (!ResourceLoad)
    {
        ResourceLoad = UAssetManager::GetStreamableManager().RequestAsyncLoad(TArray<FSoftObjectPath>{StandingPath, RiflePath});
    }
    if (ResourceLoad && !ResourceLoad->HasLoadCompleted())
    {
        return false;
    }
    auto *Standing = Cast<UAnimSequence>(StandingPath.ResolveObject());
    auto *Rifle = Cast<UBBBEquipmentDefinition>(RiflePath.ResolveObject());
    const auto *Character = Cast<ABBBCharacter>(Pawn);
    if (!Character || !Standing || !Rifle || !Rifle->EquipmentMesh)
    {
        return false;
    }
    FActorSpawnParameters Spawn;
    Spawn.ObjectFlags |= RF_Transient;
    Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    PreviewScene = MakeShared<FPreviewScene>(FPreviewScene::ConstructionValues().SetEditor(false)
        .SetCreateDefaultLighting(false).AllowAudioPlayback(false).SetCreatePhysicsScene(false)
        .ShouldSimulatePhysics(false).SetTransactional(false));
    World = PreviewScene->GetWorld();
    Display = World->SpawnActor<AActor>(AActor::StaticClass(), Origin, FRotator::ZeroRotator, Spawn);
    Capture = World->SpawnActor<ASceneCapture2D>(ASceneCapture2D::StaticClass(), Origin, FRotator::ZeroRotator, Spawn);
    if (!Display || !Capture)
    {
        Close();
        return false;
    }
    Display->SetReplicates(false);
    Capture->SetReplicates(false);
    USceneComponent *Root = NewObject<USceneComponent>(Display);
    Display->AddInstanceComponent(Root);
    Display->SetRootComponent(Root);
    Root->RegisterComponent();
    Root->SetWorldLocation(Origin);
    BuildRoom(*Root);
    Pose = NewObject<USkeletalMeshComponent>(Display);
    Display->AddInstanceComponent(Pose);
    Pose->SetSkeletalMesh(Character->GetMesh()->GetSkeletalMeshAsset());
    Pose->SetForcedLOD(1);
    Pose->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
    Pose->SetVisibility(false);
    Pose->SetVisibleInSceneCaptureOnly(true);
    Pose->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Pose->SetOverlayMaterial(nullptr);
    Pose->SetRenderCustomDepth(false);
    Pose->RegisterComponent();
    FTransform StandingTransform = Character->GetMesh()->GetRelativeTransform();
    StandingTransform.AddToTranslation(Origin + FVector(0.0f, 0.0f, Pawn->GetSimpleCollisionHalfHeight()));
    Pose->SetWorldTransform(StandingTransform);
    Pose->PlayAnimation(Standing, true);
    Pose->SetPosition(0.0f, false);
    Pose->TickAnimation(1.0f / 30.0f, false);
    Pose->RefreshBoneTransforms();
    Pose->SetComponentTickEnabled(false);
    const FName Grip = Character->GetCharacterConfig().Interaction.RightHandSocketName;
    if (ensureMsgf(Pose->DoesSocketExist(Grip), TEXT("整备间 PLA 骨架缺少挂接点 %s"), *Grip.ToString()))
    {
        auto* Gun = NewObject<USkeletalMeshComponent>(Display);
        Display->AddInstanceComponent(Gun);
        Gun->SetSkeletalMesh(Rifle->EquipmentMesh);
        Gun->SetupAttachment(Pose, Grip);
        Gun->SetRelativeTransform(Rifle->SpawnOffset);
        Gun->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Gun->SetForcedLOD(1);
        Gun->RegisterComponent();
    }

    Texture = NewObject<UTextureRenderTarget2D>(this);
    Texture->ClearColor = FLinearColor::Black;
    Texture->RenderTargetFormat = RTF_RGBA8_SRGB;
    Texture->Filter = TF_Bilinear;
    Texture->InitAutoFormat(1920, 1080);
    auto *Camera = Capture->GetCaptureComponent2D();
    Camera->TextureTarget = Texture;
    Camera->bCaptureEveryFrame = false;
    Camera->bCaptureOnMovement = false;
    Camera->bAlwaysPersistRenderingState = true;
    Camera->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    Camera->ShowOnlyActors.Add(Display);
    Camera->ShowFlags.SetAtmosphere(false);
    Camera->ShowFlags.SetFog(true);
    Camera->ShowFlags.SetVolumetricFog(true);
    Camera->ShowFlags.SetSkyLighting(false);
    Camera->ShowFlags.SetLighting(true);
    Camera->ShowFlags.SetDynamicShadows(true);
    Camera->ShowFlags.SetBloom(true);
    Camera->ShowFlags.SetMotionBlur(false);
    Camera->ShowFlags.SetScreenSpaceReflections(false);
    Camera->ShowFlags.SetLumenReflections(false);
    Camera->ShowFlags.SetReflectionEnvironment(false);
    Camera->ShowFlags.SetTemporalAA(true);
    Camera->PostProcessSettings.bOverride_DynamicGlobalIlluminationMethod = true;
    Camera->PostProcessSettings.DynamicGlobalIlluminationMethod = EDynamicGlobalIlluminationMethod::None;
    Camera->PostProcessSettings.bOverride_ReflectionMethod = true;
    Camera->PostProcessSettings.ReflectionMethod = EReflectionMethod::None;
    Camera->ProjectionType = ECameraProjectionMode::Perspective;
    Camera->FOVAngle = 49.0f;
    Camera->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    Camera->PostProcessSettings.bOverride_AutoExposureMethod = true;
    Camera->PostProcessSettings.AutoExposureMethod = AEM_Manual;
    Camera->PostProcessSettings.bOverride_AutoExposureBias = true;
    Camera->PostProcessSettings.AutoExposureBias = -1.18f;
    Camera->PostProcessSettings.bOverride_AutoExposureApplyPhysicalCameraExposure = true;
    Camera->PostProcessSettings.AutoExposureApplyPhysicalCameraExposure = false;
    Camera->PostProcessSettings.bOverride_BloomIntensity = true;
    Camera->PostProcessSettings.BloomIntensity = 0.48f;
    Camera->PostProcessSettings.bOverride_FilmGrainIntensity = true;
    Camera->PostProcessSettings.FilmGrainIntensity = 0.0f;
    HomeCamera = FTransform(FRotator(-4.5f, 180.0f, 0.0f), Origin + FVector(525.0f, 0.0f, 139.0f));
    Capture->SetActorTransform(HomeCamera);
    bDirty = true;
    return true;
}

void UBBBPlayerItemPortrait::ApplyCameraOrbit(FVector2D Orbit)
{
    const FQuat BaseRotation = HomeCamera.GetRotation();
    const FVector BasePosition = HomeCamera.GetLocation();
    const FVector Pivot = Origin + FVector(0.0, 0.0, 112.0);
    const FQuat Offset = FRotator(Orbit.Y * 1.05, Orbit.X * 1.6, 0.0).Quaternion();
    const FQuat Rotation = BaseRotation * Offset;
    const FQuat WorldOffset = Rotation * BaseRotation.Inverse();
    Capture->SetActorLocationAndRotation(Pivot + WorldOffset.RotateVector(BasePosition - Pivot), Rotation);
}

void UBBBPlayerItemPortrait::Update(float DeltaTime, const TOptional<FVector2D>& Cursor)
{
    if (!bActive || !Source.IsValid() || !IsValid(Display) || !IsValid(Capture) || !IsValid(Pose)
        || !PreviewScene.IsValid())
    {
        return;
    }
    const auto *Character = Cast<ABBBCharacter>(Source.Get());
    const uint64 Revision = Character ? Character->RuntimeData.Appearance.ReadCharacterAppearanceDisplayState().AppliedRevision : 0;
    FVector2D TargetOrbit = Cursor.Get(DisplayedOrbit);
#if WITH_DEV_AUTOMATION_TESTS
    if (ReviewPoseTime >= 0.0f)
    {
        TargetOrbit = FVector2D::ZeroVector;
    }
#endif
    constexpr float CaptureInterval = 1.0f / 60.0f;
    CaptureCredit += DeltaTime;
    PendingPoseTime += DeltaTime;
    const bool bNeedsSync = bDirty || Revision != ObservedAppearanceRevision;
    if (!bNeedsSync && CaptureCredit + UE_KINDA_SMALL_NUMBER < CaptureInterval)
    {
        return;
    }
    CaptureCredit = CaptureCredit >= CaptureInterval ? FMath::Fmod(CaptureCredit, CaptureInterval) : 0.0f;

#if WITH_DEV_AUTOMATION_TESTS
    if (ReviewPoseTime >= 0.0f)
    {
        Pose->SetPosition(ReviewPoseTime, false);
        Pose->TickAnimation(0.0f, false);
    }
    else
#endif
    {
        Pose->TickAnimation(PendingPoseTime, false);
    }
    Pose->RefreshBoneTransforms();

    if (bNeedsSync)
    {
        SyncAppearance(Character);
        ObservedAppearanceRevision = Revision;
    }
    for (UWidgetComponent* Panel : Panels)
    {
        Panel->TickComponent(PendingPoseTime, LEVELTICK_All, nullptr);
    }

    const FVector2D NextOrbit = DisplayedOrbit.Equals(TargetOrbit, 0.001)
        ? TargetOrbit
        : FMath::Lerp(DisplayedOrbit, TargetOrbit, 1.0 - FMath::Exp(-16.0 * PendingPoseTime));
    PendingPoseTime = 0.0f;
    // 命中检测读取对应这次捕获的机位
    if (NextOrbit != DisplayedOrbit)
    {
        ApplyCameraOrbit(NextOrbit);
        DisplayedOrbit = NextOrbit;
    }
    Capture->GetCaptureComponent2D()->CaptureScene();
#if WITH_DEV_AUTOMATION_TESTS
    ++CaptureRevision;
#endif
    bDirty = false;
}

void UBBBPlayerItemPortrait::SyncAppearance(const ABBBCharacter* Character)
{
    TArray<USkeletalMeshComponent *> Actual;
    Source->GetComponents(Actual);
    while (Components.Num() > Actual.Num())
    {
        Components.Last()->DestroyComponent();
        Components.Pop();
    }
    for (int32 Index = 0; Index < Actual.Num(); ++Index)
    {
        if (!Components.IsValidIndex(Index))
        {
            USkeletalMeshComponent *Part = NewObject<USkeletalMeshComponent>(Display);
            Display->AddInstanceComponent(Part);
            Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            Part->SetVisibleInSceneCaptureOnly(true);
            Part->SetLightingChannels(true, false, false);
            Part->SetOverlayMaterial(nullptr);
            Part->SetRenderCustomDepth(false);
            Part->SetComponentTickEnabled(false);
            Part->RegisterComponent();
            Components.Add(Part);
        }
        auto *Part = Components[Index].Get();
        auto *Original = Actual[Index];
        const bool bMeshChanged = Part->GetSkeletalMeshAsset() != Original->GetSkeletalMeshAsset();
        if (bMeshChanged)
        {
            Part->EmptyOverrideMaterials();
            Part->SetSkeletalMesh(Original->GetSkeletalMeshAsset());
            Part->SetForcedLOD(1);
            Part->SetOverlayMaterial(nullptr);
        }
        if (!Original->GetSkeletalMeshAsset())
        {
            Part->SetVisibility(false);
            continue;
        }
        const bool bVisible = Original->IsVisible() && !Original->bHiddenInGame && !Original->GetOwner()->IsHidden();
        if (Part->IsVisible() != bVisible)
        {
            Part->SetVisibility(bVisible);
        }
        bool bSurfaceChanged = bMeshChanged;
        for (int32 MaterialIndex = 0; MaterialIndex < Original->GetNumMaterials(); ++MaterialIndex)
        {
            if (Part->GetMaterial(MaterialIndex) != Original->GetMaterial(MaterialIndex))
            {
                Part->SetMaterial(MaterialIndex, Original->GetMaterial(MaterialIndex));
                bSurfaceChanged = true;
            }
        }
        if (bVisible && bSurfaceChanged)
        {
            Part->PrestreamTextures(4.0f, true);
        }
        const FName Socket = Original->GetAttachSocketName();
        const bool bRigid = !Socket.IsNone() && !Original->LeaderPoseComponent.IsValid();
        USkeletalMeshComponent *Leader = bRigid ? nullptr : Pose.Get();
        if (Part->LeaderPoseComponent.Get() != Leader)
        {
            Part->SetLeaderPoseComponent(Leader);
        }
        const bool bPoseChanged = Part->bForceRefpose != bRigid;
        if (bPoseChanged)
        {
            Part->SetForceRefPose(bRigid);
        }
        if (bRigid && (bMeshChanged || bPoseChanged))
        {
            Part->RefreshBoneTransforms();
        }
        const bool bBody = Character && Original == Character->GetMesh();
        const bool bBodyAttachment = Character && Original->GetAttachParent() == Character->GetMesh();
        if (bBody || bBodyAttachment)
        {
            const FName PreviewSocket = bBody ? NAME_None : Socket;
            if (Part->GetAttachParent() != Pose || Part->GetAttachSocketName() != PreviewSocket)
            {
                Part->AttachToComponent(Pose, FAttachmentTransformRules::KeepRelativeTransform, PreviewSocket);
            }
            Part->SetRelativeTransform(bBody ? FTransform::Identity : Original->GetRelativeTransform());
            continue;
        }

        if (Part->GetAttachParent())
        {
            Part->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        }
        FTransform Relative = Original->GetComponentTransform().GetRelativeTransform(Source->GetActorTransform());
        Relative.AddToTranslation(Origin + FVector(0.0f, 0.0f, Source->GetSimpleCollisionHalfHeight()));
        Part->SetWorldTransform(Relative);
    }
}

void UBBBPlayerItemPortrait::Close()
{
    if (ResourceLoad)
    {
        ResourceLoad->CancelHandle();
    }
    ResourceLoad.Reset();
    if (IsValid(Capture))
    {
        Capture->Destroy();
    }
    if (IsValid(Display))
    {
        Display->Destroy();
    }
    Components.Reset();
    Pose = nullptr;
    Capture = nullptr;
    Display = nullptr;
    RoomKey = nullptr;
    RoomMist = nullptr;
    Texture = nullptr;
    Panels.Reset();
    PreviewScene.Reset();
    Source.Reset();
    DisplayedOrbit = FVector2D::ZeroVector;
    CaptureCredit = 0.0f;
    PendingPoseTime = 0.0f;
    bActive = false;
    bDirty = true;
    ObservedAppearanceRevision = 0;
}
void UBBBPlayerItemPortrait::BeginDestroy()
{
    Close();
    Super::BeginDestroy();
}

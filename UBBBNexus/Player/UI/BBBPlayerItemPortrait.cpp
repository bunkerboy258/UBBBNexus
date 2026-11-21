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
    Elapsed = 0.0f;
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
    Capture->SetActorLocation(Origin + FVector(525.0f, 0.0f, 139.0f));
    Capture->SetActorRotation(FRotator(-4.5f, 180.0f, 0.0f));
    bDirty = true;
    return true;
}

void UBBBPlayerItemPortrait::Update(float DeltaTime)
{
    if (!bActive || !Source.IsValid() || !IsValid(Display) || !IsValid(Capture) || !IsValid(Pose)
        || !PreviewScene.IsValid())
    {
        return;
    }
    const auto *Character = Cast<ABBBCharacter>(Source.Get());
    const uint64 Revision = Character ? Character->RuntimeData.Appearance.ReadCharacterAppearanceDisplayState().AppliedRevision : 0;
    Elapsed += DeltaTime;
    // 打开背包和穿戴结果变化时立即同步 静止时才限制检查频率
    if (!bDirty && Revision == ObservedAppearanceRevision && Elapsed < 1.0f / 12.0f)
    {
        return;
    }
#if WITH_DEV_AUTOMATION_TESTS
    if (ReviewPoseTime >= 0.0f)
    {
        Pose->SetPosition(ReviewPoseTime, false);
        Pose->TickAnimation(0.0f, false);
    }
    else
#endif
    {
        Pose->TickAnimation(Elapsed, false);
    }
    Pose->RefreshBoneTransforms();
    Elapsed = 0.0f;
    ObservedAppearanceRevision = Revision;
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
        FTransform Relative = Original->GetComponentTransform().GetRelativeTransform(Source->GetActorTransform());
        Relative.AddToTranslation(FVector(0.0f, 0.0f, Source->GetSimpleCollisionHalfHeight()));
        Relative.AddToTranslation(Origin);
        if (Character && Original == Character->GetMesh())
        {
            Relative = Pose->GetComponentTransform();
        }
        if (Character && Original->GetAttachParent() == Character->GetMesh())
        {
            Relative = Original->GetRelativeTransform() * Pose->GetSocketTransform(Socket, RTS_World);
        }
        if (!Part->GetComponentTransform().Equals(Relative))
        {
            Part->SetWorldTransform(Relative);
        }
    }
    for (UWidgetComponent* Panel : Panels)
    {
        Panel->TickComponent(DeltaTime, LEVELTICK_All, nullptr);
    }
    Capture->GetCaptureComponent2D()->CaptureScene();
#if WITH_DEV_AUTOMATION_TESTS
    ++CaptureRevision;
#endif
    bDirty = false;
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
    Elapsed = 0.0f;
    bActive = false;
    bDirty = true;
    ObservedAppearanceRevision = 0;
}
void UBBBPlayerItemPortrait::BeginDestroy()
{
    Close();
    Super::BeginDestroy();
}

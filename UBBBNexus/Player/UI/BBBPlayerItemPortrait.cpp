#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/PointLightComponent.h"
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
#include "Materials/MaterialInstanceDynamic.h"
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
    if (IsValid(Display))
    {
        TArray<UPointLightComponent *> Lights;
        Display->GetComponents(Lights);
        for (auto *Light : Lights)
        {
            Light->SetVisibility(bActive);
        }
    }
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
    const FSoftObjectPath StandingPath(TEXT("/Game/_Project/Characters/BBBC_UA/Animation/UsedAnimation/unarmed/MM_Unarmed_Idle_Ready.MM_Unarmed_Idle_Ready"));
    const FSoftObjectPath MaterialPath(TEXT("/Game/_Project/UI/Bag/M_CharacterPreview.M_CharacterPreview"));
    if (!ResourceLoad)
    {
        ResourceLoad = UAssetManager::GetStreamableManager().RequestAsyncLoad(TArray<FSoftObjectPath>{StandingPath, MaterialPath});
    }
    if (ResourceLoad && !ResourceLoad->HasLoadCompleted())
    {
        return false;
    }
    auto *Standing = Cast<UAnimSequence>(StandingPath.ResolveObject());
    auto *Parent = Cast<UMaterialInterface>(MaterialPath.ResolveObject());
    const auto *Character = Cast<ABBBCharacter>(Pawn);
    if (!Character || !Standing || !Parent)
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
    struct FPortraitLight
    {
        FVector Location;
        FLinearColor Color;
        float Candela;
        bool bShadow;
    };
    const FPortraitLight Lights[] = {
        {FVector(90.0f, -123.0f, 211.0f), FLinearColor(1.0f, 0.84f, 0.68f), 28.0f, true},
        {FVector(-75.0f, 70.0f, 188.0f), FLinearColor(0.62f, 0.80f, 1.0f), 31.0f, false},
        {FVector(147.0f, 55.0f, 137.0f), FLinearColor(1.0f, 0.95f, 0.91f), 4.5f, false},
    };
    for (const FPortraitLight &Rig : Lights)
    {
        UPointLightComponent *Light = NewObject<UPointLightComponent>(Display);
        Display->AddInstanceComponent(Light);
        Light->SetupAttachment(Root);
        Light->SetLightingChannels(true, false, false);
        Light->SetCastShadows(Rig.bShadow);
        Light->SetAttenuationRadius(420.0f);
        Light->SetIntensityUnits(ELightUnits::Candelas);
        Light->SetIntensity(Rig.Candela);
        Light->SetSpecularScale(0.28f);
        Light->SetSourceRadius(Rig.bShadow ? 8.0f : 28.0f);
        Light->SetSoftSourceRadius(Rig.bShadow ? 14.0f : 40.0f);
        Light->ContactShadowLength = Rig.bShadow ? 0.12f : 0.0f;
        Light->ShadowResolutionScale = 2.0f;
        Light->SetLightColor(Rig.Color);
        Light->SetRelativeLocation(Rig.Location);
        Light->SetVisibility(bActive);
        Light->RegisterComponent();
    }
    auto *Floor = NewObject<UStaticMeshComponent>(Display);
    Display->AddInstanceComponent(Floor);
    Floor->SetupAttachment(Root);
    Floor->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane")));
    Floor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Floor->SetLightingChannels(true, false, false);
    Floor->SetCastShadow(false);
    Floor->SetRelativeScale3D(FVector(0.58f, 0.36f, 1.0f));
    Floor->SetRelativeLocation(FVector(8.0f, 0.0f, -1.2f));
    Floor->RegisterComponent();
    Floor->SetMaterial(0, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/_Project/UI/Bag/M_InventoryGround.M_InventoryGround")));
    Pose = NewObject<USkeletalMeshComponent>(Display);
    Display->AddInstanceComponent(Pose);
    Pose->SetSkeletalMesh(Character->GetMesh()->GetSkeletalMeshAsset());
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
    Texture = NewObject<UTextureRenderTarget2D>(this);
    Texture->ClearColor = FLinearColor::Transparent;
    Texture->RenderTargetFormat = RTF_RGBA16f;
    Texture->InitAutoFormat(640, 1080);
    Texture->SRGB = false;

    Material = UMaterialInstanceDynamic::Create(Parent, this);
    Material->SetTextureParameterValue(TEXT("Preview"), Texture);
    Material->SetScalarParameterValue(TEXT("Exposure"), 0.55f);
    auto *Camera = Capture->GetCaptureComponent2D();
    Camera->TextureTarget = Texture;
    Camera->bCaptureEveryFrame = false;
    Camera->bCaptureOnMovement = false;
    Camera->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    Camera->ShowOnlyActors.Add(Display);
    Camera->ShowFlags.SetAtmosphere(false);
    Camera->ShowFlags.SetFog(false);
    Camera->ShowFlags.SetSkyLighting(false);
    Camera->ShowFlags.SetLighting(true);
    Camera->ShowFlags.SetDynamicShadows(true);
    Camera->ShowFlags.SetBloom(false);
    Camera->ShowFlags.SetScreenSpaceReflections(false);
    Camera->ShowFlags.SetLumenReflections(false);
    Camera->ShowFlags.SetReflectionEnvironment(false);
    Camera->ShowFlags.SetTemporalAA(false);
    Camera->PostProcessSettings.bOverride_DynamicGlobalIlluminationMethod = true;
    Camera->PostProcessSettings.DynamicGlobalIlluminationMethod = EDynamicGlobalIlluminationMethod::None;
    Camera->PostProcessSettings.bOverride_ReflectionMethod = true;
    Camera->PostProcessSettings.ReflectionMethod = EReflectionMethod::None;
    Camera->ProjectionType = ECameraProjectionMode::Orthographic;
    Camera->OrthoWidth = 130.0f;
    Camera->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
    Camera->PostProcessSettings.bOverride_AutoExposureMethod = true;
    Camera->PostProcessSettings.AutoExposureMethod = AEM_Manual;
    Camera->PostProcessSettings.bOverride_AutoExposureBias = true;
    Camera->PostProcessSettings.AutoExposureBias = 0.35f;
    Camera->PostProcessSettings.bOverride_AutoExposureApplyPhysicalCameraExposure = true;
    Camera->PostProcessSettings.AutoExposureApplyPhysicalCameraExposure = false;
    Camera->PostProcessSettings.bOverride_BloomIntensity = true;
    Camera->PostProcessSettings.BloomIntensity = 0.0f;
    Capture->SetActorLocation(Origin + FVector(300.0f, 0.0f, 113.0f));
    Capture->SetActorRotation(FRotator(-5.0f, 180.0f, 0.0f));
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
    Pose->TickAnimation(Elapsed, false);
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
        for (int32 MaterialIndex = 0; MaterialIndex < Original->GetNumMaterials(); ++MaterialIndex)
        {
            if (Part->GetMaterial(MaterialIndex) != Original->GetMaterial(MaterialIndex))
            {
                Part->SetMaterial(MaterialIndex, Original->GetMaterial(MaterialIndex));
            }
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
    PreviewScene->GetWorld()->SendAllEndOfFrameUpdates();
    Capture->GetCaptureComponent2D()->CaptureScene();
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
    Texture = nullptr;
    Material = nullptr;
    PreviewScene.Reset();
    Source.Reset();
    Elapsed = 0.0f;
    bActive = false;
    bDirty = true;
    ObservedAppearanceRevision = 0;
}
UMaterialInstanceDynamic *UBBBPlayerItemPortrait::GetMaterial() const
{
    return Material;
}
void UBBBPlayerItemPortrait::BeginDestroy()
{
    Close();
    Super::BeginDestroy();
}

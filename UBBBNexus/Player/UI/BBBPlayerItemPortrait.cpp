#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemStage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/WidgetComponent.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Config/BBBEquipmentDefinition.h"
#include "PreviewScene.h"
#include "Components/SceneComponent.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/Level.h"
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
    CameraOffsetVelocity = FVector2D::ZeroVector;
}

bool UBBBPlayerItemPortrait::IsForPawn(const APawn* Pawn) const
{
    return Source.IsValid() && Source.Get() == Pawn;
}

const USceneCaptureComponent2D* UBBBPlayerItemPortrait::GetCamera() const
{
    return Stage ? Stage->GetCapture() : nullptr;
}

bool UBBBPlayerItemPortrait::PrepareStage()
{
    if (bStageLoadFailed)
    {
        return false;
    }
    if (!PreviewScene)
    {
        PreviewScene = MakeShared<FPreviewScene>(FPreviewScene::ConstructionValues().SetEditor(false)
            .SetCreateDefaultLighting(false).AllowAudioPlayback(false).SetCreatePhysicsScene(false)
            .ShouldSimulatePhysics(false).SetTransactional(false));
        const TSoftObjectPtr<UWorld> Map(FSoftObjectPath(TEXT("/Game/_Project/UI/Bag/Scenes/L_InventoryStudio.L_InventoryStudio")));
        bool bAccepted = false;
        StageLevel = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
            PreviewScene->GetWorld(), Map, FTransform::Identity, bAccepted, FString(), nullptr, true);
        if (!bAccepted || !StageLevel)
        {
            UE_LOG(LogTemp, Error, TEXT("无法加载整备间关卡 %s"), *Map.ToString());
            bStageLoadFailed = true;
            return false;
        }
    }
    PreviewScene->GetWorld()->UpdateLevelStreaming();
    if (StageLevel->GetLevelStreamingState() == ELevelStreamingState::FailedToLoad)
    {
        UE_LOG(LogTemp, Error, TEXT("整备间关卡流送失败"));
        bStageLoadFailed = true;
        return false;
    }
    if (!StageLevel->IsLevelVisible())
    {
        return false;
    }
    if (!Stage)
    {
        for (AActor* Actor : StageLevel->GetLoadedLevel()->Actors)
        {
            auto* Candidate = Cast<ABBBPlayerItemStage>(Actor);
            if (Candidate && Stage)
            {
                UE_LOG(LogTemp, Error, TEXT("整备间关卡只能放置一个整备间摆件"));
                Stage = nullptr;
                bStageLoadFailed = true;
                return false;
            }
            if (Candidate)
            {
                Stage = Candidate;
            }
        }
        if (!Stage)
        {
            UE_LOG(LogTemp, Error, TEXT("整备间关卡缺少整备间摆件"));
            bStageLoadFailed = true;
            return false;
        }
        Stage->UpdateMist();
    }
    return true;
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
    if (IsValid(Display) && IsValid(Stage) && IsValid(Pose))
    {
        return true;
    }
    const FSoftObjectPath StandingPath(TEXT("/Game/_Project/Characters/Animation/OtherAnimation/Project/AS_Rifle_Idle_GunDown_SG550.AS_Rifle_Idle_GunDown_SG550"));
    const FSoftObjectPath RiflePath(TEXT("/Game/_Project/Characters/Items/Equipment/Rifle/SG550/DA_ModernWeapons_Rifle_02.DA_ModernWeapons_Rifle_02"));
    if (!ResourceLoad)
    {
        ResourceLoad = UAssetManager::GetStreamableManager().RequestAsyncLoad(TArray<FSoftObjectPath>{StandingPath, RiflePath});
    }
    if (!PrepareStage() || (ResourceLoad && !ResourceLoad->HasLoadCompleted()))
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
    World = PreviewScene->GetWorld();
    const FTransform CharacterTransform = Stage->GetCharacterAnchor().GetComponentTransform();
    Display = World->SpawnActor<AActor>(AActor::StaticClass(), CharacterTransform, Spawn);
    if (!Display)
    {
        Close();
        return false;
    }
    Display->SetReplicates(false);
    USceneComponent *Root = NewObject<USceneComponent>(Display);
    Display->AddInstanceComponent(Root);
    Display->SetRootComponent(Root);
    Root->RegisterComponent();
    Root->SetWorldTransform(CharacterTransform);
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
    StandingTransform.AddToTranslation(FVector(0.0f, 0.0f, Pawn->GetSimpleCollisionHalfHeight()));
    Pose->SetWorldTransform(StandingTransform * CharacterTransform);
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
    auto* Camera = Stage->GetCapture();
    Camera->TextureTarget = Texture;
    for (AActor* Actor : StageLevel->GetLoadedLevel()->Actors)
    {
        if (Actor && Actor->IsEditorOnly())
        {
            Camera->HiddenActors.AddUnique(Actor);
        }
    }
    HomeCamera = Camera->GetComponentTransform();
    bDirty = true;
    return true;
}

void UBBBPlayerItemPortrait::ApplyCameraParallax(FVector2D Offset)
{
    const FQuat BaseRotation = HomeCamera.GetRotation();
    const FVector BasePosition = HomeCamera.GetLocation();
    const FVector Pivot = Stage->GetOrbitAnchor().GetComponentLocation();
    const FQuat Rotation = BaseRotation * FRotator(Offset.Y * 0.39, Offset.X * 0.85, 0.0).Quaternion();
    const FQuat WorldOffset = Rotation * BaseRotation.Inverse();
    const FVector Truck = Rotation.GetRightVector() * (-1.6 * Offset.X) + Rotation.GetUpVector() * (-0.8 * Offset.Y);
    Stage->GetCapture()->SetWorldLocationAndRotation(Pivot + WorldOffset.RotateVector(BasePosition - Pivot) + Truck, Rotation);
}

void UBBBPlayerItemPortrait::Update(float DeltaTime, const TOptional<FVector2D>& Cursor)
{
    if (!bActive || !Source.IsValid() || !IsValid(Display) || !IsValid(Stage) || !IsValid(Pose)
        || !PreviewScene.IsValid())
    {
        return;
    }
    const auto *Character = Cast<ABBBCharacter>(Source.Get());
    const uint64 Revision = Character ? Character->RuntimeData.Appearance.ReadCharacterAppearanceDisplayState().AppliedRevision : 0;
    TOptional<FVector2D> TargetOffset = Cursor;
#if WITH_DEV_AUTOMATION_TESTS
    if (ReviewPoseTime >= 0.0f)
    {
        TargetOffset = FVector2D::ZeroVector;
    }
#endif
    const bool bNeedsSync = bDirty || Revision != ObservedAppearanceRevision;
    if (!bNeedsSync && DeltaTime <= 0.0f)
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
        Pose->TickAnimation(DeltaTime, false);
    }
    Pose->RefreshBoneTransforms();

    if (bNeedsSync)
    {
        SyncAppearance(Character);
        ObservedAppearanceRevision = Revision;
    }
    for (UWidgetComponent* Panel : Panels)
    {
        Panel->TickComponent(DeltaTime, LEVELTICK_All, nullptr);
    }

    const FVector2D PreviousOffset = CameraOffset;
    if (TargetOffset.IsSet())
    {
        FMath::SpringDamperSmoothing(CameraOffset, CameraOffsetVelocity, TargetOffset.GetValue(),
            FVector2D::ZeroVector, DeltaTime, 0.073f, 1.0f);
        if (CameraOffset.Equals(TargetOffset.GetValue(), 0.001) && CameraOffsetVelocity.IsNearlyZero(0.015))
        {
            CameraOffset = TargetOffset.GetValue();
            CameraOffsetVelocity = FVector2D::ZeroVector;
        }
    }
    else
    {
        CameraOffsetVelocity = FVector2D::ZeroVector;
    }
    // 命中检测读取对应这次捕获的机位
    if (CameraOffset != PreviousOffset)
    {
        ApplyCameraParallax(CameraOffset);
    }
    Stage->GetCapture()->CaptureScene();
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
            Part->SetLightingChannels(true, true, false);
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
        Relative.AddToTranslation(FVector(0.0f, 0.0f, Source->GetSimpleCollisionHalfHeight()));
        Part->SetWorldTransform(Relative * Stage->GetCharacterAnchor().GetComponentTransform());
    }
}

void UBBBPlayerItemPortrait::Close()
{
    if (ResourceLoad)
    {
        ResourceLoad->CancelHandle();
    }
    ResourceLoad.Reset();
    if (IsValid(Display))
    {
        Display->Destroy();
    }
    Components.Reset();
    Pose = nullptr;
    Stage = nullptr;
    Display = nullptr;
    StageLevel = nullptr;
    Texture = nullptr;
    Panels.Reset();
    PreviewScene.Reset();
    Source.Reset();
    CameraOffset = FVector2D::ZeroVector;
    CameraOffsetVelocity = FVector2D::ZeroVector;
    bActive = false;
    bDirty = true;
    bStageLoadFailed = false;
    ObservedAppearanceRevision = 0;
}
void UBBBPlayerItemPortrait::BeginDestroy()
{
    Close();
    Super::BeginDestroy();
}

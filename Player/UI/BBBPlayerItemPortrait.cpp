#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPortrait.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Animation/AnimSequence.h"
#include "Materials/MaterialInstanceDynamic.h"

bool UBBBPlayerItemPortrait::Open(APawn *Pawn)
{
    Close();
    if (!IsValid(Pawn))
    {
        return false;
    }
    Source = Pawn;
    UWorld *World = Pawn->GetWorld();
    if (!World || World->IsNetMode(NM_DedicatedServer))
    {
        return false;
    }
    FActorSpawnParameters Spawn;
    Spawn.ObjectFlags |= RF_Transient;
    Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    Origin = Pawn->GetActorLocation() + FVector(0.0f, 0.0f, 10000.0f);
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
    for (int32 Index = 0; Index < 2; ++Index)
    {
        UPointLightComponent *Light = NewObject<UPointLightComponent>(Display);
        Display->AddInstanceComponent(Light);
        Light->SetupAttachment(Root);
        Light->SetLightingChannels(false, true, false);
        Light->SetCastShadows(false);
        Light->SetAttenuationRadius(650.0f);
        Light->SetIntensity(Index == 0 ? 2800.0f : 1100.0f);
        Light->SetLightColor(Index == 0 ? FLinearColor(1.0f, 0.91f, 0.78f) : FLinearColor(0.68f, 0.78f, 1.0f));
        Light->SetRelativeLocation(Index == 0 ? FVector(190.0f, -155.0f, 240.0f) : FVector(100.0f, 170.0f, 130.0f));
        Light->RegisterComponent();
    }
    const ABBBCharacter *Character = Cast<ABBBCharacter>(Pawn);
    UAnimSequence *Standing = LoadObject<UAnimSequence>(
        nullptr, TEXT("/Game/_Project/Characters/BBBC_UA/Animation/Lyra/MM_Unarmed_Idle_Ready.MM_Unarmed_Idle_Ready"));
    if (!ensureMsgf(Character && Standing, TEXT("固定人物预览缺少角色或站姿动画")))
    {
        Close();
        return false;
    }
    Pose = NewObject<USkeletalMeshComponent>(Display);
    Display->AddInstanceComponent(Pose);
    Pose->SetSkeletalMesh(Character->GetMesh()->GetSkeletalMeshAsset());
    Pose->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
    Pose->SetVisibility(false);
    Pose->SetVisibleInSceneCaptureOnly(true);
    Pose->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Pose->RegisterComponent();
    Pose->PlayAnimation(Standing, false);
    Pose->SetPosition(0.0f, false);
    Pose->TickAnimation(1.0f / 30.0f, false);
    Pose->RefreshBoneTransforms();
    Texture = NewObject<UTextureRenderTarget2D>(this);
    Texture->ClearColor = FLinearColor::Transparent;
    Texture->RenderTargetFormat = RTF_RGBA16f;
    Texture->InitAutoFormat(640, 1080);
    Texture->UpdateResourceImmediate(true);
    UMaterialInterface *Parent =
        LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/_Project/UI/Bag/M_CharacterPreview.M_CharacterPreview"));
    if (!ensureMsgf(Parent, TEXT("人物预览材质缺失")))
    {
        Close();
        return false;
    }
    Material = UMaterialInstanceDynamic::Create(Parent, this);
    Material->SetTextureParameterValue(TEXT("Preview"), Texture);
    auto *Camera = Capture->GetCaptureComponent2D();
    Camera->TextureTarget = Texture;
    Camera->bCaptureEveryFrame = false;
    Camera->bCaptureOnMovement = false;
    Camera->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    Camera->ShowOnlyActors.Add(Display);
    Camera->ShowFlags.SetAtmosphere(false);
    Camera->ShowFlags.SetFog(false);
    Camera->ShowFlags.SetSkyLighting(false);
    Camera->ShowFlags.SetDynamicShadows(false);
    Camera->PostProcessSettings.bOverride_DynamicGlobalIlluminationMethod = true;
    Camera->PostProcessSettings.DynamicGlobalIlluminationMethod = EDynamicGlobalIlluminationMethod::None;
    Camera->ProjectionType = ECameraProjectionMode::Orthographic;
    Camera->OrthoWidth = 125.0f;
    Camera->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
    Camera->PostProcessSettings.bOverride_AutoExposureMethod = true;
    Camera->PostProcessSettings.AutoExposureMethod = AEM_Manual;
    Capture->SetActorLocation(Origin + FVector(300.0f, 0.0f, 90.0f));
    Capture->SetActorRotation(FRotator(0.0f, 180.0f, 0.0f));
    Update(1.0f);
    return true;
}

void UBBBPlayerItemPortrait::Update(float DeltaTime)
{
    if (!Source.IsValid() || !IsValid(Display) || !IsValid(Capture) || Source->GetWorld() != Capture->GetWorld())
    {
        return;
    }
    Elapsed += DeltaTime;
    if (Elapsed < 1.0f / 20.0f)
    {
        return;
    }
    Elapsed = 0.0f;
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
            UPoseableMeshComponent *Part = NewObject<UPoseableMeshComponent>(Display);
            Display->AddInstanceComponent(Part);
            Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            Part->SetVisibleInSceneCaptureOnly(true);
            Part->SetLightingChannels(false, true, false);
            Part->RegisterComponent();
            Components.Add(Part);
        }
        auto *Part = Components[Index].Get();
        auto *Original = Actual[Index];
        if (Part->GetSkinnedAsset() != Original->GetSkeletalMeshAsset())
        {
            Part->SetSkinnedAssetAndUpdate(Original->GetSkeletalMeshAsset());
        }
        if (!Original->GetSkeletalMeshAsset())
        {
            Part->SetVisibility(false);
            continue;
        }
        Part->SetVisibility(Original->IsVisible() && !Original->GetOwner()->IsHidden());
        for (int32 MaterialIndex = 0; MaterialIndex < Original->GetNumMaterials(); ++MaterialIndex)
        {
            Part->SetMaterial(MaterialIndex, Original->GetMaterial(MaterialIndex));
        }
        const FName Socket = Original->GetAttachSocketName();
        const bool bRigid = !Socket.IsNone() && !Original->LeaderPoseComponent.IsValid();
        Part->CopyPoseFromSkeletalComponent(bRigid ? Original : Pose.Get());
        Part->RefreshBoneTransforms();
        FTransform Relative = Original->GetComponentTransform().GetRelativeTransform(Source->GetActorTransform());
        const ABBBCharacter *Character = Cast<ABBBCharacter>(Source.Get());
        if (bRigid && Character && Original->GetAttachParent() == Character->GetMesh())
        {
            Relative = Original->GetRelativeTransform() * Pose->GetSocketTransform(Socket, RTS_Component) *
                       Character->GetMesh()->GetRelativeTransform();
        }
        Relative.AddToTranslation(FVector(0.0f, 0.0f, Source->GetSimpleCollisionHalfHeight()));
        Relative.AddToTranslation(Origin);
        Part->SetWorldTransform(Relative);
    }
    Capture->GetCaptureComponent2D()->CaptureScene();
}

void UBBBPlayerItemPortrait::Close()
{
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
    Source.Reset();
    Elapsed = 0.0f;
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

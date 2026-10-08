#include "BBBWork/UBBBNexus/Player/UI/BBBPlayerItemPreview.h"
#include "PreviewScene.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "Animation/AnimSequence.h"
#include "Materials/MaterialInstanceDynamic.h"

bool UBBBPlayerItemPreview::Open(APawn *Pawn)
{
    Close();
    if (!IsValid(Pawn))
    {
        return false;
    }
    Source = Pawn;
    FPreviewScene::ConstructionValues Options;
    Options.SetEditor(false).SetCreatePhysicsScene(false).SetForceMipsResident(false);
    Scene = MakeUnique<FPreviewScene>(Options);
    Scene->DirectionalLight->SetIntensity(3.0f);
    Scene->DirectionalLight->SetWorldRotation(FRotator(-25.0f, 160.0f, 0.0f));
    Display = Scene->GetWorld()->SpawnActor<AActor>();
    Capture = Scene->GetWorld()->SpawnActor<ASceneCapture2D>();
    if (!Display || !Capture)
    {
        Close();
        return false;
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
    Camera->ProjectionType = ECameraProjectionMode::Orthographic;
    Camera->OrthoWidth = 125.0f;
    Camera->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
    Camera->PostProcessSettings.bOverride_AutoExposureMethod = true;
    Camera->PostProcessSettings.AutoExposureMethod = AEM_Manual;
    Capture->SetActorLocation(FVector(300.0f, 0.0f, 90.0f));
    Capture->SetActorRotation(FRotator(0.0f, 180.0f, 0.0f));
    Update(1.0f);
    return true;
}

void UBBBPlayerItemPreview::Update(float DeltaTime)
{
    if (!Scene || !Source.IsValid() || !Capture)
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
            Part->RegisterComponent();
            Components.Add(Part);
        }
        auto *Part = Components[Index].Get();
        auto *Original = Actual[Index];
        if (Part->GetSkinnedAsset() != Original->GetSkeletalMeshAsset())
        {
            Part->SetSkinnedAssetAndUpdate(Original->GetSkeletalMeshAsset());
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
        Part->SetWorldTransform(Relative);
    }
    Capture->GetCaptureComponent2D()->CaptureScene();
}

void UBBBPlayerItemPreview::Close()
{
    Components.Reset();
    Pose = nullptr;
    Capture = nullptr;
    Display = nullptr;
    Texture = nullptr;
    Material = nullptr;
    Source.Reset();
    Scene.Reset();
}
UTextureRenderTarget2D *UBBBPlayerItemPreview::GetTexture() const
{
    return Texture;
}
UMaterialInstanceDynamic *UBBBPlayerItemPreview::GetMaterial() const
{
    return Material;
}
void UBBBPlayerItemPreview::BeginDestroy()
{
    Close();
    Super::BeginDestroy();
}

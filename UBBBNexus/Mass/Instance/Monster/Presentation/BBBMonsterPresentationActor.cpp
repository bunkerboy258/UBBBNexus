#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "BBBMonsterPhysicalAnimationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "BBBMonsterSoundPresentationComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "BBBMonsterBloodPresentationSubsystem.h"
#include "BBBMonsterGibPresentationSubsystem.h"
#include "BBBMonsterGorePresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"

ABBBMonsterPresentationActor::ABBBMonsterPresentationActor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> BurningAsset(
        TEXT("/Game/_Project/Mass/Monster/Effects/NS_Monster_Burning_Wisps_V2.NS_Monster_Burning_Wisps_V2"));
    BurningSystem = BurningAsset.Object;
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BurningOverlayAsset(
        TEXT("/Game/_Project/Mass/Monster/Effects/M_Monster_Burning_Overlay.M_Monster_Burning_Overlay"));
    BurningOverlayMaterial = BurningOverlayAsset.Object;
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("MonsterRoot"));
    SetRootComponent(Root);
    MonsterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MonsterMesh"));
    MonsterMesh->BodyInstance.SetOverrideIterationCounts(true);
    MonsterMesh->BodyInstance.SetPositionSolverIterationCount(8);
    MonsterMesh->BodyInstance.SetVelocitySolverIterationCount(2);
    MonsterMesh->BodyInstance.SetProjectionSolverIterationCount(2);
    MonsterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MonsterMesh->SetCanEverAffectNavigation(false);
    MonsterMesh->SetGenerateOverlapEvents(false);
    MonsterMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    // 关闭动态光追 BLAS 重建与 MeshSDF 间接光照，避免几十只高模僵尸拖垮渲染线程与 GPU
    MonsterMesh->SetVisibleInRayTracing(false);
    MonsterMesh->bAffectDistanceFieldLighting = false;
    MonsterMesh->bAffectDynamicIndirectLighting = false;
    MonsterMesh->bEnableUpdateRateOptimizations = true;
    MonsterMesh->bSkipKinematicUpdateWhenInterpolating = true;
    MonsterMesh->bSkipBoundsUpdateWhenInterpolating = true;
    MonsterMesh->SetupAttachment(Root);
    MonsterMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    MonsterPresentation = CreateDefaultSubobject<UBBBMonsterPresentationComponent>(TEXT("MonsterPresentation"));
    Gore = CreateDefaultSubobject<UBBBMonsterGorePresentationComponent>(TEXT("MonsterGore"));
    PhysicalAnimation = CreateDefaultSubobject<UBBBMonsterPhysicalAnimationComponent>(TEXT("MonsterPhysicalAnimation"));
    PhysicalAnimation->PrimaryComponentTick.bStartWithTickEnabled = false;
    HitReaction = CreateDefaultSubobject<UBBBMonsterHitReactionComponent>(TEXT("MonsterHitReaction"));
    SoundPresentation = CreateDefaultSubobject<UBBBMonsterSoundPresentationComponent>(TEXT("MonsterSoundPresentation"));
    SoundPresentation->SetupAttachment(Root);
    ContactAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("MonsterContactAudio"));
    ContactAudio->SetupAttachment(Root);
    ContactAudio->bAutoActivate = false;
    ContactAudio->bAutoDestroy = false;
    ContactAudio->bStopWhenOwnerDestroyed = true;
    ContactAudio->bCanPlayMultipleInstances = false;
    ContactAudio->PrimaryComponentTick.bCanEverTick = false;
}

bool ABBBMonsterPresentationActor::ConfigurePresentation(const UBBBMonsterPresentationDefinition* Settings)
{
    if (!Settings)
    {
        return false;
    }
    if (PresentationSettings == Settings)
    {
        return true;
    }
    if (!Settings->IsValid())
    {
        return false;
    }
    HitReaction->Deactivate();
    MonsterPresentation->ResetCorpsePresentation();
    Gore->ResetPresentation();
    MonsterMesh->EmptyOverrideMaterials();
    MonsterMesh->SetSkeletalMesh(Settings->Gore->Body);
    MonsterMesh->SetPhysicsAsset(Settings->PhysicsAsset, true);
    MonsterMesh->SetRelativeTransform(Settings->MeshTransform);
    MonsterMesh->SetAnimInstanceClass(Settings->AnimationClass);
    HitReaction->ConfigurePresentation(Settings);
    if (auto *Gibs = GetWorld()->GetSubsystem<UBBBMonsterGibPresentationSubsystem>())
    {
        Gibs->Prepare(*Settings);
    }
    if (Settings->Blood)
    {
        if (auto *Blood = GetWorld()->GetSubsystem<UBBBMonsterBloodPresentationSubsystem>())
        {
            Blood->Prepare(*Settings->Blood);
        }
    }
    PresentationSettings = Settings;
    return true;
}

USkeletalMeshComponent* ABBBMonsterPresentationActor::GetMonsterMesh() const
{
    return MonsterMesh;
}

UBBBMonsterPresentationComponent* ABBBMonsterPresentationActor::GetMonsterPresentation() const
{
    return MonsterPresentation;
}

UBBBMonsterSoundPresentationComponent* ABBBMonsterPresentationActor::GetMonsterSoundPresentation() const
{
    return SoundPresentation;
}

UAudioComponent* ABBBMonsterPresentationActor::GetMonsterContactAudio() const
{
    return ContactAudio;
}

UBBBMonsterGorePresentationComponent* ABBBMonsterPresentationActor::GetMonsterGore() const
{
    return Gore;
}

void ABBBMonsterPresentationActor::SetActorHiddenInGame(const bool bNewHidden)
{
    Super::SetActorHiddenInGame(bNewHidden);
    if (bNewHidden)
    {
        SetBurningOverlay(false);
        SetBurningPresentation(false);
    }
    if (bNewHidden && SoundPresentation)
    {
        SoundPresentation->ResetPresentation();
    }
    if (bNewHidden && ContactAudio)
    {
        ContactAudio->Stop();
    }
    if (bNewHidden && MonsterPresentation)
    {
        MonsterPresentation->ResetCorpsePresentation();
    }
    if (bNewHidden && Gore)
    {
        Gore->ResetPresentation();
    }
}

bool ABBBMonsterPresentationActor::HasBurningPresentation() const
{
    return BurningEffect != nullptr;
}

void ABBBMonsterPresentationActor::SetBurningOverlay(bool bBurning)
{
    if (bBurningOverlayActive == bBurning || !MonsterMesh)
    {
        return;
    }
    bBurningOverlayActive = bBurning;
    MonsterMesh->SetOverlayMaterial(bBurning ? BurningOverlayMaterial.Get() : nullptr);
}

void ABBBMonsterPresentationActor::SetBurningPresentation(bool bBurning)
{
    if (!bBurning)
    {
        if (BurningEffect)
        {
            if (BurningEffect->PoolingMethod == ENCPoolMethod::ManualRelease)
            {
                BurningEffect->DeactivateImmediate();
                BurningEffect->ReleaseToPool();
            }
            else
            {
                BurningEffect->DestroyComponent();
            }
            BurningEffect = nullptr;
        }
        return;
    }
    if (BurningEffect || !BurningSystem || !MonsterMesh)
    {
        return;
    }
    // 使用全身立式细碎小火舌 + 上升余烬包络（无块状篝火贴片感）
    BurningEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(BurningSystem, MonsterMesh,
        NAME_None, FVector(0.0, 0.0, 82.0), FRotator::ZeroRotator, FVector(0.55),
        EAttachLocation::KeepRelativeOffset, false, ENCPoolMethod::ManualRelease);
    if (BurningEffect)
    {
        BurningEffect->SetVariableVec3(TEXT("User.Size Of Campfire"), FVector(65.0, 65.0, 190.0));
        BurningEffect->SetVariableLinearColor(TEXT("User.Linear Color"), FLinearColor(2.4f, 0.42f, 0.04f, 1.0f));
        if (!BurningEffect->IsActive())
        {
            BurningEffect->Activate(true);
        }
    }
}

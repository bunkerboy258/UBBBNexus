#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "BBBMonsterPhysicalAnimationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "BBBMonsterSoundPresentationComponent.h"
#include "Components/AudioComponent.h"
#include "BBBMonsterGorePresentationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterPresentationDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterGoreDefinition.h"

ABBBMonsterPresentationActor::ABBBMonsterPresentationActor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("MonsterRoot"));
    SetRootComponent(Root);
    MonsterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MonsterMesh"));
    MonsterMesh->BodyInstance.SetOverrideIterationCounts(true);
    MonsterMesh->BodyInstance.SetPositionSolverIterationCount(32);
    MonsterMesh->BodyInstance.SetVelocitySolverIterationCount(8);
    MonsterMesh->BodyInstance.SetProjectionSolverIterationCount(8);
    MonsterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MonsterMesh->SetGenerateOverlapEvents(false);
    MonsterMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
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

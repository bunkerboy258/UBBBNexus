#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "BBBMonsterPhysicalAnimationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "BBBMonsterSoundPresentationComponent.h"
#include "Components/AudioComponent.h"
#include "BBBMonsterSeveringPresentationComponent.h"

ABBBMonsterPresentationActor::ABBBMonsterPresentationActor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("MonsterRoot"));
    SetRootComponent(Root);
    MonsterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MonsterMesh"));
    MonsterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MonsterMesh->SetGenerateOverlapEvents(false);
    MonsterMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    MonsterMesh->SetupAttachment(Root);
    MonsterMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    MonsterPresentation = CreateDefaultSubobject<UBBBMonsterPresentationComponent>(TEXT("MonsterPresentation"));
    Severing = CreateDefaultSubobject<UBBBMonsterSeveringPresentationComponent>(TEXT("MonsterSevering"));
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

UBBBMonsterSeveringPresentationComponent* ABBBMonsterPresentationActor::GetMonsterSevering() const
{
    return Severing;
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
    if (bNewHidden && Severing)
    {
        Severing->ResetPresentation();
    }
}

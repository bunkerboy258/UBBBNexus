#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterHitReactionComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"
#include "BBBMonsterSoundPresentationComponent.h"

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
    PhysicalAnimation = CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("MonsterPhysicalAnimation"));
    PhysicalAnimation->PrimaryComponentTick.bStartWithTickEnabled = false;
    HitReaction = CreateDefaultSubobject<UBBBMonsterHitReactionComponent>(TEXT("MonsterHitReaction"));
    SoundPresentation = CreateDefaultSubobject<UBBBMonsterSoundPresentationComponent>(TEXT("MonsterSoundPresentation"));
    SoundPresentation->SetupAttachment(Root);
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

void ABBBMonsterPresentationActor::SetActorHiddenInGame(const bool bNewHidden)
{
    Super::SetActorHiddenInGame(bNewHidden);
    if (bNewHidden && SoundPresentation)
    {
        SoundPresentation->ResetPresentation();
    }
}

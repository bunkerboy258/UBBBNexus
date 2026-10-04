#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationActor.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterPresentationComponent.h"

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
    MonsterMesh->SetupAttachment(Root);
    MonsterMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    MonsterPresentation = CreateDefaultSubobject<UBBBMonsterPresentationComponent>(TEXT("MonsterPresentation"));
}

USkeletalMeshComponent* ABBBMonsterPresentationActor::GetMonsterMesh() const
{
    return MonsterMesh;
}

UBBBMonsterPresentationComponent* ABBBMonsterPresentationActor::GetMonsterPresentation() const
{
    return MonsterPresentation;
}

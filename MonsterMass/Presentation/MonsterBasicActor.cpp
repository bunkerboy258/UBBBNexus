#include "BBBWork/UBBBNexus/MonsterMass/Presentation/MonsterBasicActor.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "MassActorSubsystem.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/MonsterMass/Presentation/MonsterPresentationComponent.h"
#include "BBBWork/UBBBNexus/MonsterMass/MonsterRuntimeData.h"

AMonsterBasicActor::AMonsterBasicActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    SetReplicateMovement(false);

    MonsterCollision = CreateDefaultSubobject<USphereComponent>(TEXT("MonsterCollision"));
    MonsterCollision->InitSphereRadius(45.0f);
    MonsterCollision->SetCollisionProfileName(TEXT("Pawn"));
    MonsterCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MonsterCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    MonsterCollision->SetGenerateOverlapEvents(true);
    SetRootComponent(MonsterCollision);

    MonsterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MonsterMesh"));
    MonsterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MonsterMesh->SetGenerateOverlapEvents(false);
    MonsterMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    MonsterMesh->SetupAttachment(MonsterCollision);

    MonsterPresentation = CreateDefaultSubobject<UMonsterPresentationComponent>(TEXT("MonsterPresentation"));
}

float AMonsterBasicActor::TakeDamage(
    const float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    const float AcceptedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (DamageAmount <= 0.0f)
    {
        return AcceptedDamage;
    }

    UWorld* World = GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster damage requires a valid world")))
    {
        return AcceptedDamage;
    }

    UMassEntitySubsystem* EntitySubsystem = UWorld::GetSubsystem<UMassEntitySubsystem>(World);
    UMassActorSubsystem* ActorSubsystem = UWorld::GetSubsystem<UMassActorSubsystem>(World);

    if (!ensureMsgf(EntitySubsystem != nullptr && ActorSubsystem != nullptr, TEXT("[UBBBM]Monster damage requires Mass entity and actor subsystems")))
    {
        return AcceptedDamage;
    }

    const FMassEntityHandle Entity = ActorSubsystem->GetEntityHandleFromActor(this);

    if (!ensureMsgf(Entity.IsValid(), TEXT("[UBBBM]Monster actor requires an associated Mass entity")))
    {
        return AcceptedDamage;
    }

    FMassEntityManager& EntityManager = EntitySubsystem->GetMutableEntityManager();
    FMonsterDamageEventFragment* DamageEventFragment = EntityManager.GetFragmentDataPtr<FMonsterDamageEventFragment>(Entity);

    if (!ensureMsgf(DamageEventFragment != nullptr, TEXT("[UBBBM]Monster entity requires MonsterDamageEventFragment")))
    {
        return AcceptedDamage;
    }

    DamageEventFragment->PendingDamage += DamageAmount;

    UE_LOG(
        LogTemp,
        Log,
        TEXT("[UBBBM]Monster hurt Actor=%s Damage=%.1f Causer=%s"),
        *GetNameSafe(this),
        DamageAmount,
        *GetNameSafe(DamageCauser));

    return DamageAmount;
}

USkeletalMeshComponent* AMonsterBasicActor::GetMonsterMesh() const
{
    return MonsterMesh;
}

UMonsterPresentationComponent* AMonsterBasicActor::GetMonsterPresentation() const
{
    return MonsterPresentation;
}

void AMonsterBasicActor::SetCollisionRadius(const float Radius)
{
    MonsterCollision->SetSphereRadius(FMath::Max(Radius, 1.0f));
    MonsterCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

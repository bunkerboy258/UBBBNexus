#include "BBBWork/UBBBNexus/MonsterMass/Presentation/MonsterBasicActor.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "MassActorSubsystem.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/MonsterMass/Presentation/MonsterPresentationComponent.h"
#include "BBBWork/UBBBNexus/MonsterMass/Entity/MonsterRuntimeData.h"

AMonsterBasicActor::AMonsterBasicActor()
{
    // 表现 Actor 的位置和生命周期由 Mass Representation 管理
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    SetReplicateMovement(false);

    // 根碰撞体只在表现层需要时开启查询碰撞
    MonsterCollision = CreateDefaultSubobject<USphereComponent>(TEXT("MonsterCollision"));
    MonsterCollision->InitSphereRadius(45.0f);
    MonsterCollision->SetCollisionProfileName(TEXT("Pawn"));
    MonsterCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MonsterCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    MonsterCollision->SetGenerateOverlapEvents(true);
    SetRootComponent(MonsterCollision);

    // 网格关闭自身碰撞，避免与根碰撞体重复计算
    MonsterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MonsterMesh"));
    MonsterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MonsterMesh->SetGenerateOverlapEvents(false);
    MonsterMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    MonsterMesh->SetupAttachment(MonsterCollision);

    // 状态组件负责把 Mass 状态转换为动画
    MonsterPresentation = CreateDefaultSubobject<UMonsterPresentationComponent>(TEXT("MonsterPresentation"));
}

float AMonsterBasicActor::TakeDamage(
    const float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser)
{
    const float AcceptedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // 无效伤害不进入实体事件
    if (DamageAmount <= 0.0f)
    {
        return AcceptedDamage;
    }

    UWorld* World = GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[UBBBM]Monster damage requires a valid world")))
    {
        return AcceptedDamage;
    }

    // 通过表现演员反查对应实体
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

    // 取得实体受伤事件片段
    FMassEntityManager& EntityManager = EntitySubsystem->GetMutableEntityManager();
    FMonsterDamageEventFragment* DamageEventFragment = EntityManager.GetFragmentDataPtr<FMonsterDamageEventFragment>(Entity);

    if (!ensureMsgf(DamageEventFragment != nullptr, TEXT("[UBBBM]Monster entity requires MonsterDamageEventFragment")))
    {
        return AcceptedDamage;
    }

    // 只写入事件 Fragment，实际扣血交给 Mass 受伤处理器
    DamageEventFragment->PendingDamage += DamageAmount;

    UE_LOG(
        LogTemp,
        Log,
        TEXT("[UBBBM]Monster hurt Actor=%s Damage=%.1f Causer=%s"),
        *GetNameSafe(this),
        DamageAmount,
        *GetNameSafe(DamageCauser));

    // 返回已接受的伤害数值
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
    // 表现同步时动态应用实体的避让半径
    MonsterCollision->SetSphereRadius(FMath::Max(Radius, 1.0f));
    MonsterCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

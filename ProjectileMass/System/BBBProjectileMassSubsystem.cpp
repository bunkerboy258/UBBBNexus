#include "BBBWork/UBBBNexus/ProjectileMass/System/BBBProjectileMassSubsystem.h"

#include "MassCommonFragments.h"
#include "MassEntityManager.h"
#include "MassEntityTemplate.h"
#include "MassMovementFragments.h"
#include "MassSpawnerSubsystem.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Config/BBBProjectileDefinition.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileMassConfigAsset.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileRuntimeFragment.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Requests/BBBProjectileSpawnRequest.h"

bool UBBBProjectileMassSubsystem::SubmitSpawnRequest(const FBBBProjectileSpawnRequest& Request)
{
    if (!ensureMsgf(IsInGameThread(), TEXT("[ProjectileMass] 弹丸只能从游戏线程创建")))
    {
        return false;
    }

    if (!ensureMsgf(Request.IsValid(), TEXT("[ProjectileMass] 弹丸出生请求缺少有效定义、网格或伤害来源")))
    {
        return false;
    }

    UWorld* World = GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[ProjectileMass] 无法获取弹丸所属世界")))
    {
        return false;
    }

    UMassSpawnerSubsystem* Spawner = World->GetSubsystem<UMassSpawnerSubsystem>();

    if (!ensureMsgf(Spawner != nullptr, TEXT("[ProjectileMass] 当前世界未启用Mass实体生成子系统")))
    {
        return false;
    }

    const FMassEntityTemplate& EntityTemplate = Request.Definition->EntityConfig->GetOrCreateEntityTemplate(*World);

    if (!ensureMsgf(EntityTemplate.IsValid(), TEXT("[ProjectileMass] 弹丸Mass实体模板无效")))
    {
        return false;
    }

    TArray<FMassEntityHandle> Entities;
    TSharedPtr<FMassEntityManager::FEntityCreationContext> CreationContext = Spawner->SpawnEntities(EntityTemplate, 1, Entities);

    if (!ensureMsgf(CreationContext.IsValid() && Entities.Num() == 1, TEXT("[ProjectileMass] Mass未能创建单枚弹丸实体")))
    {
        return false;
    }

    FMassEntityManager& EntityManager = Spawner->GetEntityManagerChecked();
    const FMassEntityHandle Entity = Entities[0];
    FTransformFragment* TransformFragment = EntityManager.GetFragmentDataPtr<FTransformFragment>(Entity);
    FMassVelocityFragment* VelocityFragment = EntityManager.GetFragmentDataPtr<FMassVelocityFragment>(Entity);
    FBBBProjectileRuntimeFragment* RuntimeFragment = EntityManager.GetFragmentDataPtr<FBBBProjectileRuntimeFragment>(Entity);

    if (!ensureMsgf(TransformFragment != nullptr && VelocityFragment != nullptr && RuntimeFragment != nullptr, TEXT("[ProjectileMass] 弹丸模板缺少运行时片段")))
    {
        Spawner->DestroyEntities(Entities);
        return false;
    }

    FTransform ProjectileTransform = Request.MuzzleTransform;
    ProjectileTransform.SetScale3D(FVector::OneVector);
    TransformFragment->GetMutableTransform() = ProjectileTransform;
    VelocityFragment->Value = ProjectileTransform.GetUnitAxis(EAxis::X) * Request.Definition->InitialSpeedCmPerSecond;

    RuntimeFragment->PreviousLocation = ProjectileTransform.GetLocation();
    RuntimeFragment->RemainingLifetimeSeconds = Request.Definition->MaximumLifetimeSeconds;
    RuntimeFragment->Damage = Request.Definition->BaseDamage;
    RuntimeFragment->PenetrationDamageMultiplier = Request.Definition->PenetrationDamageMultiplier;
    RuntimeFragment->RemainingPenetrations = Request.Definition->MaximumPenetrations;
    RuntimeFragment->CollisionRadiusCm = Request.Definition->CollisionRadiusCm;
    RuntimeFragment->CollisionChannel = Request.Definition->CollisionChannel;
    RuntimeFragment->DamageCauser = Request.DamageCauser;
    RuntimeFragment->InstigatorPawn = Request.InstigatorPawn;
    RuntimeFragment->EventInstigator = Request.EventInstigator;
    RuntimeFragment->LastHitActor.Reset();
    RuntimeFragment->bPendingDestroy = false;

    return true;
}

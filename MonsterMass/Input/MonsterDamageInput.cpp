#include "BBBWork/UBBBNexus/MonsterMass/Input/MonsterDamageInput.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "MassActorSubsystem.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "BBBWork/UBBBNexus/MonsterMass/Events/MonsterDamageEvent.h"
#include "BBBWork/UBBBNexus/MonsterMass/States/MonsterStateData.h"

bool UMonsterDamageInput::SubmitDamage(AActor* Target, const FMonsterDamageRequest& Request)
{
    if (!ensureMsgf(IsInGameThread(), TEXT("[UBBBM]SubmitDamage must run on the game thread")) || !IsValid(Target))
    {
        return false;
    }

    // 通过表现演员反查对应实体
    UWorld* World = Target->GetWorld();
    UMassActorSubsystem* ActorSubsystem = UWorld::GetSubsystem<UMassActorSubsystem>(World);

    if (ActorSubsystem == nullptr)
    {
        return false;
    }

    return SubmitDamageToEntity(World, ActorSubsystem->GetEntityHandleFromActor(Target), Request);
}

bool UMonsterDamageInput::SubmitDamageToEntity(UWorld* World, const FMassEntityHandle Entity, const FMonsterDamageRequest& Request)
{
    if (!ensureMsgf(IsInGameThread(), TEXT("[UBBBM]SubmitDamageToEntity must run on the game thread")))
    {
        return false;
    }

    // 无效伤害不进入实体事件
    if (!FMath::IsFinite(Request.Damage) || Request.Damage <= 0.0f || Request.HitLocation.ContainsNaN() || Request.HitDirection.ContainsNaN())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UBBBM]Rejected invalid damage input Damage=%g"), Request.Damage);
        return false;
    }

    UMassEntitySubsystem* EntitySubsystem = UWorld::GetSubsystem<UMassEntitySubsystem>(World);

    if (EntitySubsystem == nullptr)
    {
        return false;
    }

    FMassEntityManager& EntityManager = EntitySubsystem->GetMutableEntityManager();

    // 句柄的代次校验阻止回收后命中复用槽位中的新实体
    if (!EntityManager.IsEntityValid(Entity))
    {
        return false;
    }

    const FMonsterHealthFragment* Health = EntityManager.GetFragmentDataPtr<FMonsterHealthFragment>(Entity);
    const FMonsterStateFragment* State = EntityManager.GetFragmentDataPtr<FMonsterStateFragment>(Entity);

    if (Health == nullptr || State == nullptr || Health->CurrentHealth <= 0.0f || State->State == EMonsterState::Dead)
    {
        return false;
    }

    // 取得实体受伤事件片段
    FMonsterDamageEventFragment* DamageEvent = EntityManager.GetFragmentDataPtr<FMonsterDamageEventFragment>(Entity);

    if (!ensureMsgf(DamageEvent != nullptr, TEXT("[UBBBM]Monster entity requires MonsterDamageEventFragment")))
    {
        return false;
    }

    // 游戏线程只追加事件 不改变实体结构 战斗回调也不会改动正在遍历的状态
    // 只写入事件 Fragment，实际扣血交给 Mass 受伤处理器
    DamageEvent->PendingRequests.Add(Request);
    UE_LOG(LogTemp, Verbose, TEXT("[UBBBM]Damage accepted Entity=%d Damage=%.2f Queued=%d"), Entity.Index, Request.Damage, DamageEvent->PendingRequests.Num());
    return true;
}

bool UMonsterDamageInput::TryGetHealth(AActor* Target, float& CurrentHealth, float& MaxHealth, bool& bDead)
{
    CurrentHealth = 0.0f;
    MaxHealth = 0.0f;
    bDead = true;

    if (!ensureMsgf(IsInGameThread(), TEXT("[UBBBM]TryGetHealth must run on the game thread")) || !IsValid(Target))
    {
        return false;
    }

    UWorld* World = Target->GetWorld();
    UMassActorSubsystem* ActorSubsystem = UWorld::GetSubsystem<UMassActorSubsystem>(World);
    const UMassEntitySubsystem* EntitySubsystem = UWorld::GetSubsystem<UMassEntitySubsystem>(World);

    if (ActorSubsystem == nullptr || EntitySubsystem == nullptr)
    {
        return false;
    }

    const FMassEntityManager& EntityManager = EntitySubsystem->GetEntityManager();
    const FMassEntityHandle Entity = ActorSubsystem->GetEntityHandleFromActor(Target);

    if (!EntityManager.IsEntityValid(Entity))
    {
        return false;
    }

    const FMonsterHealthFragment* Health = EntityManager.GetFragmentDataPtr<FMonsterHealthFragment>(Entity);
    const FMonsterStateFragment* State = EntityManager.GetFragmentDataPtr<FMonsterStateFragment>(Entity);

    if (Health == nullptr || State == nullptr)
    {
        return false;
    }

    CurrentHealth = Health->CurrentHealth;
    MaxHealth = Health->MaxHealth;
    bDead = CurrentHealth <= 0.0f || State->State == EMonsterState::Dead;
    return true;
}

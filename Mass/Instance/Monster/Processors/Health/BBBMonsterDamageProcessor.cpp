#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Health/BBBMonsterDamageProcessor.h"
#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterDamageFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthInputFragment.h"
#include "MassEntitySubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Network/BBBMonsterNetworkInputFragment.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMobilityFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehaviorFragment.h"

bool UBBBMonsterDamageProcessor::Query(UWorld& World, FMassEntityHandle Entity,
    TArray<FBBBMonsterDamageContribution>& Result)
{
    Result.Reset();
    auto* Entities = World.GetSubsystem<UMassEntitySubsystem>();
    if (!IsInGameThread() || Entities == nullptr)
    {
        return false;
    }
    const auto& Manager = Entities->GetEntityManager();
    if (!Manager.IsEntityValid(Entity))
    {
        return false;
    }
    const auto* Damage = Manager.GetFragmentDataPtr<FBBBMonsterDamageFragment>(Entity);
    const auto* Input = Manager.GetFragmentDataPtr<FBBBMonsterHealthInputFragment>(Entity);
    const auto* Network = Manager.GetFragmentDataPtr<FBBBMonsterNetworkFragment>(Entity);
    if (Damage == nullptr || Input == nullptr || Network == nullptr)
    {
        return false;
    }
    FBBBMonsterDamageLocalControlPacket Snapshot;
    for (const auto& Value : Damage->Contributions)
    {
        Snapshot.Include(Value.Value);
    }
    if (Input->Damage.bActive && Input->Damage.Packet.IsValid())
    {
        for (const auto& Value : Input->Damage.Packet.Contributions)
        {
            Snapshot.Include(Value);
        }
    }
    const auto* NetworkInput = Manager.GetFragmentDataPtr<FBBBMonsterNetworkInputFragment>(Entity);
    const FGuid EffectiveId = Network->InstanceId.IsValid() ? Network->InstanceId
        : (NetworkInput != nullptr && NetworkInput->State.bActive && NetworkInput->State.Packet.IsValid()
            ? NetworkInput->State.Packet.InstanceId : FGuid());
    if (Input->RemoteDamage.bActive && Input->RemoteDamage.Packet.IsValid()
        && Input->RemoteDamage.Packet.InstanceId == EffectiveId)
    {
        for (const auto& Value : Input->RemoteDamage.Packet.Contributions)
        {
            Snapshot.Include(Value);
        }
    }
    Result = MoveTemp(Snapshot.Contributions);
    Result.Sort([](const auto& A, const auto& B)
    {
        return A.PlayerId < B.PlayerId;
    });
    return true;
}

UBBBMonsterDamageProcessor::UBBBMonsterDamageProcessor()
    : EntityQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Decision;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Parse);
}

void UBBBMonsterDamageProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    EntityQuery.AddRequirement<FBBBMonsterDamageFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBMonsterNetworkFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBMonsterMobilityFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBMonsterBehaviorFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBMonsterDamageProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    const UWorld* World = Context.GetWorld();
    const float Now = World->GetTimeSeconds();
    const AGameStateBase* GameState = World->GetGameState();
    const double ServerTime = GameState ? GameState->GetServerWorldTimeSeconds() : Now;
    EntityQuery.ForEachEntityChunk(Context, [Now, ServerTime](FMassExecutionContext& Chunk)
    {
        auto Damage = Chunk.GetMutableFragmentView<FBBBMonsterDamageFragment>();
        auto Health = Chunk.GetMutableFragmentView<FBBBMonsterHealthFragment>();
        auto Mobility = Chunk.GetMutableFragmentView<FBBBMonsterMobilityFragment>();
        const auto Network = Chunk.GetFragmentView<FBBBMonsterNetworkFragment>();
        const auto Behaviors = Chunk.GetFragmentView<FBBBMonsterBehaviorFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            const float Previous = Health[Index].CurrentHealth;
            double TotalDamage = 0.0;
            double LegDamage = 0.0;
            const FBBBMonsterDamageContribution* Latest = nullptr;
            for (const auto& Value : Damage[Index].Contributions)
            {
                TotalDamage += Value.Value.Damage;
                LegDamage += Value.Value.LegDamage;
                if (!Latest || Value.Value.LastHitTime > Latest->LastHitTime)
                {
                    Latest = &Value.Value;
                }
            }
            Health[Index].CurrentHealth = static_cast<float>(FMath::Max(0.0,
                static_cast<double>(Health[Index].MaxHealth) - TotalDamage));
            Damage[Index].bReceivedDamage = Health[Index].CurrentHealth < Previous;
            auto& Motion = Mobility[Index];
            const auto* Settings = Network[Index].Definition.Get();
            if (Health[Index].CurrentHealth <= 0.0f)
            {
                Motion.SlowMinimumRatio = 1.0f;
                Motion.SlowRecoveryStartedAt = Now;
                Motion.SlowEndsAt = Now;
                Motion.HitStopEndsAt = Now;
                Motion.StaggerEndsAt = Now;
                Motion.StaggerStartedAt = Now;
                continue;
            }
            if (!Settings)
            {
                continue;
            }
            if (!Motion.bCrawling && LegDamage + KINDA_SMALL_NUMBER >= static_cast<double>(Health[Index].MaxHealth) * Settings->CrawlLegDamageFraction)
            {
                Motion.bCrawling = true;
                Motion.CrawlStartedAt = Now;
                Motion.StaggerStartedAt = Now;
                Motion.StaggerEndsAt = Now;
                Motion.HitStopEndsAt = Now;
                UE_LOG(LogTemp, Log, TEXT("[BBBMonsterCrawl] Entity=%d LegDamage=%.2f Threshold=%.2f"),
                    Chunk.GetEntity(Index).Index, LegDamage, Health[Index].MaxHealth * Settings->CrawlLegDamageFraction);
            }
            if (!Damage[Index].bReceivedDamage || !Latest)
            {
                continue;
            }
            float Ratio = Settings->BodyHitSpeedRatio;
            float Duration = Settings->BodyHitSlowDuration;
            if (Latest->LastHitRegion == EBBBMonsterHitRegion::LeftArm || Latest->LastHitRegion == EBBBMonsterHitRegion::RightArm)
            {
                Ratio = Settings->ArmHitSpeedRatio;
                Duration = Settings->ArmHitSlowDuration;
            }
            if (Latest->LastHitRegion == EBBBMonsterHitRegion::LeftLeg || Latest->LastHitRegion == EBBBMonsterHitRegion::RightLeg)
            {
                Ratio = Settings->LegHitSpeedRatio;
                Duration = Settings->LegHitSlowDuration;
            }
            const double Age = ServerTime - Latest->LastHitTime;
            const float HoldDuration = Settings->HitSlowHoldDuration;
            if (Age < -0.1 || Age >= HoldDuration + Duration)
            {
                continue;
            }
            const float HitTime = Now - static_cast<float>(FMath::Max(Age, 0.0));
            Motion.SlowMinimumRatio = FMath::Min(Motion.GetSpeedRatio(FMath::Max(Now, Motion.HitStopEndsAt)), Ratio);
            Motion.SlowRecoveryStartedAt = FMath::Max(Motion.SlowRecoveryStartedAt, HitTime + HoldDuration);
            Motion.SlowEndsAt = FMath::Max(Motion.SlowEndsAt, Motion.SlowRecoveryStartedAt + Duration);
            if (Behaviors[Index].State == EBBBMonsterBehavior::Attack)
            {
                continue;
            }
            if (Motion.bCrawling)
            {
                Motion.HitStopEndsAt = FMath::Max(Motion.HitStopEndsAt, HitTime + Settings->HitStopDuration);
                continue;
            }
            // 当前动作完成后保留短暂恢复窗口 不按每颗子弹重启姿势
            if (Age < Settings->StaggerDuration && Now >= Motion.StaggerEndsAt + (Motion.StaggerEndsAt > 0.0f ? 0.2f : 0.0f))
            {
                Motion.StaggerStartedAt = HitTime;
                Motion.StaggerEndsAt = HitTime + Settings->StaggerDuration;
                Motion.StaggerRegion = Latest->LastHitRegion;
                Motion.HitStopEndsAt = HitTime + FMath::Max(Settings->HitStopDuration, Settings->StaggerDuration * 0.7f);
                Motion.SlowRecoveryStartedAt = FMath::Max(Motion.SlowRecoveryStartedAt, Motion.HitStopEndsAt);
                Motion.SlowEndsAt = FMath::Max(Motion.SlowEndsAt, Motion.SlowRecoveryStartedAt + Duration);
                UE_LOG(LogTemp, Verbose, TEXT("[BBBMonsterStagger] Entity=%d Region=%d Start=%.3f StopEnd=%.3f End=%.3f"),
                    Chunk.GetEntity(Index).Index, static_cast<int32>(Motion.StaggerRegion), Motion.StaggerStartedAt, Motion.HitStopEndsAt, Motion.StaggerEndsAt);
            }
        }
    });
}

#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Collision/BBBProjectileCollisionProcessor.h"

#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "NiagaraDataChannel.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Movement/BBBProjectileMotionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Collision/BBBProjectileCollisionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Lifetime/BBBProjectileLifetimeFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Presentation/BBBProjectilePresentationFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Collision/BBBProjectileImpact.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Presentation/BBBProjectilePresentation.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Health/FBBBMonsterDamageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/HitReaction/FBBBMonsterHitReactionLocalControlPacket.h"

namespace
{
    void AccumulateDamage(UWorld& World, UBBBMassSubsystem& Mass,
        TMap<FMassEntityHandle, FBBBMonsterDamageLocalControlPacket>& Results,
        const FBBBProjectileCollisionFragment& Data, FMassEntityHandle Target, uint8 Part,
        const FVector& Position, const FVector& Direction, const FVector& Normal)
    {
        FBBBMonsterHitReactionLocalControlPacket Hit;
        Hit.Region = static_cast<EBBBMonsterHitRegion>(Part);
        Hit.Position = Position;
        Hit.Direction = Direction;
        Hit.Normal = Normal;
        Mass.SubmitInput(Target, MoveTemp(Hit));
        if (!Data.bCanCauseDamage || Data.Damage <= 0.0f)
        {
            return;
        }

        const AController* Controller = Data.EventInstigator.Get();
        const APlayerState* Player = Controller != nullptr ? Controller->GetPlayerState<APlayerState>() : nullptr;
        if (!ensureMsgf(Player != nullptr && Player->GetPlayerId() >= 0,
            TEXT("有效子弹伤害需要稳定的 PlayerState 玩家身份")))
        {
            return;
        }

        auto* Result = Results.Find(Target);
        if (Result == nullptr)
        {
            FBBBMonsterDamageLocalControlPacket Snapshot;
            if (!Mass.QueryDamage(Target, Snapshot.Contributions))
            {
                return;
            }
            Result = &Results.Add(Target, MoveTemp(Snapshot));
        }

        auto* Contribution = Result->Contributions.FindByPredicate(
            [Player](const auto& Value) { return Value.PlayerId == Player->GetPlayerId(); });
        FBBBMonsterDamageContribution Current = Contribution ? *Contribution : FBBBMonsterDamageContribution{};
        Current.PlayerId = Player->GetPlayerId();
        Current.Damage += Data.Damage;
        if (Part == static_cast<uint8>(EBBBMonsterHitRegion::LeftLeg) || Part == static_cast<uint8>(EBBBMonsterHitRegion::RightLeg))
        {
            Current.LegDamage += Data.Damage;
        }
        const auto* GameState = World.GetGameState();
        Current.LastHitTime = GameState ? GameState->GetServerWorldTimeSeconds() : World.GetTimeSeconds();
        Current.LastHitRegion = static_cast<EBBBMonsterHitRegion>(Part);
        Current.bHasSourcePosition = Controller->GetPawn() != nullptr;
        if (Current.bHasSourcePosition)
        {
            Current.LastSourcePosition = Controller->GetPawn()->GetActorLocation();
        }
        Result->Include(Current);
    }
}

UBBBProjectileCollisionProcessor::UBBBProjectileCollisionProcessor()
    : EntityQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::FrameEnd;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Collision;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Movement);
}

void UBBBProjectileCollisionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBProjectileMotionFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBProjectileCollisionFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBProjectileLifetimeFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FBBBProjectilePresentationFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBProjectileCollisionProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    UBBBMassSubsystem* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    // 只汇总本轮结果 每目标最后提交一个完整累计快照
    TMap<FMassEntityHandle, FBBBMonsterDamageLocalControlPacket> DamageResults;
    TArray<FBBBProjectileImpact> Impacts;
    TWeakObjectPtr<UNiagaraDataChannelAsset> ImpactChannel;
    const float Delta = Context.GetDeltaTimeSeconds();
    const auto Detonate = [World, Mass, &DamageResults](const FBBBProjectileCollisionFragment& Data, const FVector& Center)
    {
        FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMassExplosion), false);
        Params.AddIgnoredActor(Data.DamageCauser.Get());
        Params.AddIgnoredActor(Data.InstigatorPawn.Get());
        TArray<FBBBMassCollisionBody> Targets;
        Mass->OverlapEntities(Center, Data.ExplosionRadiusCm, Targets);
        for (const auto& Body : Targets)
        {
            const FVector Direction = (Body.Center - Center).GetSafeNormal(UE_SMALL_NUMBER, FVector::UpVector);
            const FVector Position = Body.Center - Direction * Body.Radius;
            if (World->LineTraceTestByChannel(Center, Position, ECC_Visibility, Params))
            {
                continue;
            }
            AccumulateDamage(*World, *Mass, DamageResults, Data, Body.Entity, Body.Part, Position, Direction, -Direction);
        }
        if (Data.bCanCauseDamage && Data.Damage > 0.0f)
        {
            TArray<AActor*> IgnoredActors;
            IgnoredActors.Add(Data.DamageCauser.Get());
            IgnoredActors.Add(Data.InstigatorPawn.Get());
            UGameplayStatics::ApplyRadialDamage(World, Data.Damage, Center, Data.ExplosionRadiusCm, nullptr,
                IgnoredActors, Data.DamageCauser.Get(), Data.EventInstigator.Get(), true, ECC_Visibility);
        }
    };
    EntityQuery.ForEachEntityChunk(Context, [World, Mass, Delta, &Detonate, &DamageResults, &Impacts, &ImpactChannel](FMassExecutionContext& Chunk)
    {
        auto Transforms = Chunk.GetMutableFragmentView<FTransformFragment>();
        auto Motion = Chunk.GetMutableFragmentView<FBBBProjectileMotionFragment>();
        auto Velocity = Chunk.GetMutableFragmentView<FMassVelocityFragment>();
        auto Collision = Chunk.GetMutableFragmentView<FBBBProjectileCollisionFragment>();
        auto Life = Chunk.GetMutableFragmentView<FBBBProjectileLifetimeFragment>();
        const auto Presentation = Chunk.GetFragmentView<FBBBProjectilePresentationFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& Data = Collision[Index];
            if (!Motion[Index].bInitialized || Life[Index].bPendingDestroy)
            {
                continue;
            }

            FVector Start = Motion[Index].PreviousLocation;
            const FVector End = Transforms[Index].GetTransform().GetLocation();
            const FVector Direction = Velocity[Index].Value.GetSafeNormal();
            FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMassProjectile), false);
            Params.bReturnPhysicalMaterial = true;
            Params.AddIgnoredActor(Data.DamageCauser.Get());
            Params.AddIgnoredActor(Data.InstigatorPawn.Get());
            Params.AddIgnoredActor(Data.LastHitActor.Get());
            TArray<FMassEntityHandle, TInlineAllocator<33>> IgnoredEntities;
            if (Data.LastHitEntity.IsSet())
            {
                IgnoredEntities.Add(Data.LastHitEntity);
            }

            // 穿透次数限定本帧查询上限 命中表现 Actor 的重复碰撞由表现层关闭
            for (int32 Attempt = 0; Attempt < 33 && !Start.Equals(End, UE_SMALL_NUMBER); ++Attempt)
            {
                FHitResult WorldHit;
                bool bWorldHit = false;
                if (Data.CollisionRadiusCm > UE_KINDA_SMALL_NUMBER)
                {
                    bWorldHit = World->SweepSingleByChannel(WorldHit, Start, End, FQuat::Identity,
                        Data.CollisionChannel, FCollisionShape::MakeSphere(Data.CollisionRadiusCm), Params);
                }
                else
                {
                    bWorldHit = World->LineTraceSingleByChannel(WorldHit, Start, End, Data.CollisionChannel, Params);
                }

                FMassEntityHandle Target;
                float EntityTime = 1.0f;
                FBBBProjectileImpact Impact;
                uint8 HitPart = 0;
                const bool bEntityHit = Mass->TraceEntities(Start, End, Data.CollisionRadiusCm,
                    IgnoredEntities, Target, EntityTime, Impact.Position, Impact.Normal, Impact.Surface, HitPart);
                const bool bUseEntity = bEntityHit && (!bWorldHit || EntityTime < WorldHit.Time);
                if (!bUseEntity && !bWorldHit)
                {
                    break;
                }

                const FVector HitPosition = bUseEntity ? FMath::Lerp(Start, End, EntityTime) : WorldHit.Location;
                if (!bUseEntity)
                {
                    Impact.Position = WorldHit.ImpactPoint;
                    Impact.Normal = WorldHit.ImpactNormal;
                    Impact.Surface = UPhysicalMaterial::DetermineSurfaceType(WorldHit.PhysMaterial.Get());
                }
                if (Presentation[Index].ImpactChannel.IsValid()
                    && ensureMsgf(!ImpactChannel.IsValid() || ImpactChannel == Presentation[Index].ImpactChannel,
                    TEXT("同一世界的子弹命中必须使用同一个批量通道")))
                {
                    ImpactChannel = Presentation[Index].ImpactChannel;
                    Impacts.Add(Impact);
                }
                if (Data.ExplosionRadiusCm > 0.0f)
                {
                    Transforms[Index].GetMutableTransform().SetLocation(HitPosition + Impact.Normal * 0.1f);
                    if (Data.bDetonateOnImpact)
                    {
                        Detonate(Data, Transforms[Index].GetTransform().GetLocation());
                        Life[Index].bPendingDestroy = true;
                        break;
                    }

                    if (Data.bBounceOnImpact)
                    {
                        Velocity[Index].Value = (Velocity[Index].Value - 2.0 * FVector::DotProduct(Velocity[Index].Value, Impact.Normal) * Impact.Normal) * Data.BounceRestitution;
                        if (Velocity[Index].Value.SizeSquared() >= 2500.0)
                        {
                            Transforms[Index].GetMutableTransform().SetRotation(Velocity[Index].Value.ToOrientationQuat());
                            break;
                        }
                    }
                    Velocity[Index].Value = FVector::ZeroVector;
                    Motion[Index].bResting = true;
                    break;
                }
                if (bUseEntity)
                {
                    AccumulateDamage(*World, *Mass, DamageResults, Data, Target, HitPart, Impact.Position, Direction, Impact.Normal);

                    IgnoredEntities.Add(Target);
                    Data.LastHitEntity = Target;
                }

                if (!bUseEntity)
                {
                    AActor* Actor = WorldHit.GetActor();
                    if (Data.bCanCauseDamage && IsValid(Actor))
                    {
                        UGameplayStatics::ApplyPointDamage(Actor, Data.Damage, Direction, WorldHit,
                            Data.EventInstigator.Get(), Data.DamageCauser.Get(), nullptr);
                    }

                    Params.AddIgnoredActor(Actor);
                    Data.LastHitActor = Actor;
                }

                if (Data.RemainingPenetrations <= 0 || (!bUseEntity && !WorldHit.GetActor()))
                {
                    Transforms[Index].GetMutableTransform().SetLocation(HitPosition);
                    Life[Index].bPendingDestroy = true;
                    break;
                }

                --Data.RemainingPenetrations;
                Data.Damage *= Data.PenetrationDamageMultiplier;
                Start = HitPosition + Direction * FMath::Max(Data.CollisionRadiusCm + 1.0f, 1.0f);
            }
            if (!Life[Index].bPendingDestroy && Life[Index].FuseRemainingSeconds > 0.0f
                && Life[Index].FuseRemainingSeconds <= Delta)
            {
                Detonate(Data, Transforms[Index].GetTransform().GetLocation());
                Life[Index].bPendingDestroy = true;
            }
        }
    });
    for (auto& Result : DamageResults)
    {
        Mass->SubmitInput(Result.Key, MoveTemp(Result.Value));
    }
    if (ImpactChannel.IsValid())
    {
        FBBBProjectilePresentation::PublishImpacts(*World, *ImpactChannel.Get(), Impacts);
    }
}

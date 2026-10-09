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
    EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FBBBProjectileMotionFragment>(EMassFragmentAccess::ReadOnly);
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
    EntityQuery.ForEachEntityChunk(Context, [World, Mass, &DamageResults, &Impacts, &ImpactChannel](FMassExecutionContext& Chunk)
    {
        auto Transforms = Chunk.GetMutableFragmentView<FTransformFragment>();
        const auto Motion = Chunk.GetFragmentView<FBBBProjectileMotionFragment>();
        const auto Velocity = Chunk.GetFragmentView<FMassVelocityFragment>();
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
                if (ensureMsgf(!ImpactChannel.IsValid() || ImpactChannel == Presentation[Index].ImpactChannel,
                    TEXT("同一世界的子弹命中必须使用同一个批量通道")))
                {
                    ImpactChannel = Presentation[Index].ImpactChannel;
                    Impacts.Add(Impact);
                }
                if (bUseEntity)
                {
                    FBBBMonsterHitReactionLocalControlPacket Hit;
                    Hit.Region = static_cast<EBBBMonsterHitRegion>(HitPart);
                    Hit.Position = Impact.Position;
                    Hit.Direction = Direction;
                    Hit.Normal = Impact.Normal;
                    Mass->SubmitInput(Target, MoveTemp(Hit));

                    if (Data.bCanCauseDamage && Data.Damage > 0.0f)
                    {
                        const AController* Controller = Data.EventInstigator.Get();
                        const APlayerState* Player = Controller != nullptr
                            ? Controller->GetPlayerState<APlayerState>() : nullptr;
                        if (ensureMsgf(Player != nullptr && Player->GetPlayerId() >= 0,
                            TEXT("有效子弹伤害需要稳定的 PlayerState 玩家身份")))
                        {
                            auto* Result = DamageResults.Find(Target);
                            if (Result == nullptr)
                            {
                                FBBBMonsterDamageLocalControlPacket Snapshot;
                                if (Mass->QueryDamage(Target, Snapshot.Contributions))
                                {
                                    Result = &DamageResults.Add(Target, MoveTemp(Snapshot));
                                }
                            }
                            if (Result != nullptr)
                            {
                                auto* Contribution = Result->Contributions.FindByPredicate(
                                    [Player](const auto& Value) { return Value.PlayerId == Player->GetPlayerId(); });
                                FBBBMonsterDamageContribution Current = Contribution ? *Contribution : FBBBMonsterDamageContribution{};
                                Current.PlayerId = Player->GetPlayerId();
                                Current.Damage += Data.Damage;
                                if (HitPart == static_cast<uint8>(EBBBMonsterHitRegion::LeftLeg) || HitPart == static_cast<uint8>(EBBBMonsterHitRegion::RightLeg))
                                {
                                    Current.LegDamage += Data.Damage;
                                }
                                const auto* GameState = World->GetGameState();
                                Current.LastHitTime = GameState ? GameState->GetServerWorldTimeSeconds() : World->GetTimeSeconds();
                                Current.LastHitRegion = static_cast<EBBBMonsterHitRegion>(HitPart);
                                Current.bHasSourcePosition = Controller->GetPawn() != nullptr;
                                if (Current.bHasSourcePosition)
                                {
                                    Current.LastSourcePosition = Controller->GetPawn()->GetActorLocation();
                                }
                                Result->Include(Current);
                            }
                        }
                    }

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

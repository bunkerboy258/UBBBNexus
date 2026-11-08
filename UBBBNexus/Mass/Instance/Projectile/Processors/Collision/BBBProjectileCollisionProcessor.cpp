#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Collision/BBBProjectileCollisionProcessor.h"
#include "Engine/OverlapResult.h"
#include "BBBWork/UBBBNexus/Combat/BBBHitReceiver.h"

#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
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
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterBodyPartDefinition.h"

namespace
{
    void SubmitHit(UBBBMassSubsystem& Mass, const FBBBProjectileCollisionFragment& Data, FMassEntityHandle Target,
        uint8 Part, const FVector& Position, const FVector& Direction, const FVector& Normal, bool bRadial)
    {
        if (!Data.bCanCauseDamage)
        {
            return;
        }
        const auto* Controller = Data.EventInstigator.Get();
        const auto* Player = Controller ? Controller->GetPlayerState<APlayerState>() : nullptr;
        if (!Data.bCanCauseDamage || !Player)
        {
            return;
        }
        FBBBHitContribution Hit{Data.Damage, NAME_None, Position, Direction};
        Hit.Normal = Normal;
        Hit.Part = Part;
        Hit.PlayerId = Player->GetPlayerId();
        Hit.AddDurableDamage = Data.DurableDamage;
        Hit.AddStagger = Data.StaggerValue;
        Hit.AddWound = Data.WoundStrength;
        Hit.AttackId = Data.AttackId;
        Hit.AttackTime = Data.AttackTime;
        Hit.bRadial = bRadial;
        Hit.bHasSourcePosition = Controller->GetPawn() != nullptr;
        if (Hit.bHasSourcePosition)
        {
            Hit.SourcePosition = Controller->GetPawn()->GetActorLocation();
        }
        Mass.SubmitInput(Target, FBBBMonsterDamageLocalControlPacket{{Hit}});
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
    TArray<FBBBProjectileImpact> Impacts;
    TWeakObjectPtr<UNiagaraDataChannelAsset> ImpactChannel;
    const float Delta = Context.GetDeltaTimeSeconds();
    const auto Detonate = [World, Mass](const FBBBProjectileCollisionFragment& Data, const FVector& Center)
    {
        FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMassExplosion), false);
        if (!Data.bDamageInstigator)
        {
            Params.AddIgnoredActor(Data.DamageCauser.Get());
            Params.AddIgnoredActor(Data.InstigatorPawn.Get());
        }
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
            SubmitHit(*Mass, Data, Body.Entity, Body.Part, Position, Direction, -Direction, true);
        }
        if (Data.bCanCauseDamage && Data.Damage > 0.0f)
        {
            TArray<FOverlapResult> Overlaps;
            World->OverlapMultiByObjectType(Overlaps, Center, FQuat::Identity, FCollisionObjectQueryParams::AllDynamicObjects,
                FCollisionShape::MakeSphere(Data.ExplosionRadiusCm), Params);
            TSet<AActor*> TargetsSeen;
            for (const auto& Overlap : Overlaps)
            {
                auto* Actor = Overlap.GetActor();
                if (!Actor || TargetsSeen.Contains(Actor))
                {
                    continue;
                }
                FCollisionQueryParams Visibility = Params;
                Visibility.AddIgnoredActor(Actor);
                const FVector Position = Actor->GetActorLocation();
                if (World->LineTraceTestByChannel(Center, Position, ECC_Visibility, Visibility))
                {
                    continue;
                }
                TargetsSeen.Add(Actor);
                FBBBHitContribution Hit{Data.Damage, NAME_None, Position, (Position - Center).GetSafeNormal()};
                Hit.bRadial = true;
                Hit.AttackId = Data.AttackId;
                BBBHitReceiver::Submit(Actor, Hit);
            }
        }
    };
    EntityQuery.ForEachEntityChunk(Context, [World, Mass, Delta, &Detonate, &Impacts, &ImpactChannel](FMassExecutionContext& Chunk)
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
                    SubmitHit(*Mass, Data, Target, HitPart, Impact.Position, Direction, Impact.Normal, false);

                    IgnoredEntities.Add(Target);
                    Data.LastHitEntity = Target;
                }

                if (!bUseEntity)
                {
                    AActor* Actor = WorldHit.GetActor();
                    if (Data.bCanCauseDamage && Data.Damage != 0.0f && IsValid(Actor))
                    {
                        FBBBHitContribution Hit{Data.Damage, WorldHit.BoneName, WorldHit.ImpactPoint, Direction};
                        Hit.AddWound = Data.WoundStrength;
                        Hit.Normal = WorldHit.ImpactNormal;
                        Hit.AttackId = Data.AttackId;
                        BBBHitReceiver::SubmitProjectileHit(Actor, Hit, Data.InstigatorPawn.Get());
                    }

                    Params.AddIgnoredActor(Actor);
                    Data.LastHitActor = Actor;
                }

                const bool bPenetrable = bUseEntity || Cast<APawn>(WorldHit.GetActor()) != nullptr;
                if (Data.RemainingPenetrations <= 0 || !bPenetrable)
                {
                    Transforms[Index].GetMutableTransform().SetLocation(HitPosition);
                    Life[Index].bPendingDestroy = true;
                    break;
                }

                --Data.RemainingPenetrations;
                Data.Damage *= Data.PenetrationDamageMultiplier;
                Data.DurableDamage *= Data.PenetrationDamageMultiplier;
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
    if (ImpactChannel.IsValid())
    {
        FBBBProjectilePresentation::PublishImpacts(*World, *ImpactChannel.Get(), Impacts);
    }
}

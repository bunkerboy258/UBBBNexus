#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Collision/BBBProjectileCollisionProcessor.h"
#include "Engine/OverlapResult.h"
#include "BBBWork/UBBBNexus/Combat/BBBHitReceiver.h"

#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
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

namespace
{
    constexpr float NearDistanceCm = 3000.0f;
    constexpr float MidDistanceCm = 8000.0f;
    constexpr float CullDistanceCm = 15000.0f;
    constexpr int32 NearBudget = 32;
    constexpr int32 MidBudget = 12;
    constexpr int32 FarBudget = 4;

    void SubmitHit(UBBBMassSubsystem& Mass, const FBBBProjectileCollisionFragment& Data, FMassEntityHandle Target,
        uint8 Part, const FVector& Position, const FVector& Direction, const FVector& Normal, bool bRadial)
    {
        if (!Data.bCanCauseDamage)
        {
            return;
        }
        const auto* Controller = Data.EventInstigator.Get();
        const auto* Player = Controller ? Controller->GetPlayerState<APlayerState>() : nullptr;
        if (!Player)
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

    FVector ObserverLocation = FVector::ZeroVector;
    bool bHasObserver = false;
    if (const APlayerController* Controller = World->GetFirstPlayerController())
    {
        if (const APawn* Pawn = Controller->GetPawn())
        {
            ObserverLocation = Pawn->GetActorLocation();
            bHasObserver = true;
        }
    }

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

    const auto TraceSegment = [World, Mass, &Detonate, &Impacts, &ImpactChannel](
        FTransformFragment& Transform,
        FMassVelocityFragment& Velocity,
        FBBBProjectileMotionFragment& Motion,
        FBBBProjectileCollisionFragment& Data,
        FBBBProjectileLifetimeFragment& Life,
        const FBBBProjectilePresentationFragment& Presentation,
        const FVector& StartIn,
        const FVector& End,
        const bool bApplyDamage)
    {
        FVector Start = StartIn;
        const FVector Direction = Velocity.Value.GetSafeNormal();
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

        for (int32 Attempt = 0; Attempt < 33 && !Start.Equals(End, UE_SMALL_NUMBER); ++Attempt)
        {
            FHitResult WorldHit;
            bool bWorldHit = false;
            if (Data.CollisionRadiusCm > UE_KINDA_SMALL_NUMBER)
            {
                bWorldHit = World->SweepSingleByChannel(WorldHit, Start, End, FQuat::Identity,
                    Data.CollisionChannel, FCollisionShape::MakeSphere(Data.CollisionRadiusCm), Params);
            }
            if (Data.CollisionRadiusCm <= UE_KINDA_SMALL_NUMBER)
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
            if (Presentation.ImpactChannel.IsValid()
                && ensureMsgf(!ImpactChannel.IsValid() || ImpactChannel == Presentation.ImpactChannel,
                TEXT("同一世界的子弹命中必须使用同一个批量通道")))
            {
                ImpactChannel = Presentation.ImpactChannel;
                Impacts.Add(Impact);
            }
            if (Data.ExplosionRadiusCm > 0.0f)
            {
                Transform.GetMutableTransform().SetLocation(HitPosition + Impact.Normal * 0.1f);
                if (Data.bDetonateOnImpact)
                {
                    if (bApplyDamage)
                    {
                        Detonate(Data, Transform.GetTransform().GetLocation());
                    }
                    Life.bPendingDestroy = true;
                    break;
                }

                if (Data.bBounceOnImpact)
                {
                    Velocity.Value = (Velocity.Value - 2.0 * FVector::DotProduct(Velocity.Value, Impact.Normal) * Impact.Normal) * Data.BounceRestitution;
                    if (Velocity.Value.SizeSquared() >= 2500.0)
                    {
                        Transform.GetMutableTransform().SetRotation(Velocity.Value.ToOrientationQuat());
                        break;
                    }
                }
                Velocity.Value = FVector::ZeroVector;
                Motion.bResting = true;
                break;
            }
            if (bUseEntity)
            {
                if (bApplyDamage)
                {
                    SubmitHit(*Mass, Data, Target, HitPart, Impact.Position, Direction, Impact.Normal, false);
                }

                IgnoredEntities.Add(Target);
                Data.LastHitEntity = Target;
            }

            if (!bUseEntity)
            {
                AActor* Actor = WorldHit.GetActor();
                if (bApplyDamage && Data.bCanCauseDamage && Data.Damage != 0.0f && IsValid(Actor))
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
                Transform.GetMutableTransform().SetLocation(HitPosition);
                Life.bPendingDestroy = true;
                break;
            }

            --Data.RemainingPenetrations;
            if (bApplyDamage)
            {
                Data.Damage *= Data.PenetrationDamageMultiplier;
                Data.DurableDamage *= Data.PenetrationDamageMultiplier;
            }
            Start = HitPosition + Direction * FMath::Max(Data.CollisionRadiusCm + 1.0f, 1.0f);
        }
    };

    int32 TierBudget[3] = {NearBudget, MidBudget, FarBudget};
    int32 TierSeen[3] = {};
    int32 NextCursor[3] = {TierCursor[0], TierCursor[1], TierCursor[2]};
    bool bTierExhausted[3] = {};

    EntityQuery.ForEachEntityChunk(Context,
        [World, Mass, Delta, &Detonate, &TraceSegment, &Impacts, &ImpactChannel, ObserverLocation, bHasObserver,
            &TierBudget, &TierSeen, &NextCursor, &bTierExhausted, this](FMassExecutionContext& Chunk)
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

            const FVector End = Transforms[Index].GetTransform().GetLocation();

            if (Data.bCanCauseDamage)
            {
                TraceSegment(Transforms[Index], Velocity[Index], Motion[Index], Data, Life[Index], Presentation[Index],
                    Motion[Index].PreviousLocation, End, true);
                Motion[Index].LastCheckLocation = End;
                if (!Life[Index].bPendingDestroy && Life[Index].FuseRemainingSeconds > 0.0f
                    && Life[Index].FuseRemainingSeconds <= Delta)
                {
                    Detonate(Data, Transforms[Index].GetTransform().GetLocation());
                    Life[Index].bPendingDestroy = true;
                }
                continue;
            }

            int32 Tier = 0;
            if (bHasObserver)
            {
                const float DistanceSq = FVector::DistSquared(End, ObserverLocation);
                if (DistanceSq > CullDistanceCm * CullDistanceCm)
                {
                    Life[Index].bPendingDestroy = true;
                    continue;
                }
                Tier = 2;
                if (DistanceSq <= NearDistanceCm * NearDistanceCm)
                {
                    Tier = 0;
                }
                if (DistanceSq > NearDistanceCm * NearDistanceCm && DistanceSq <= MidDistanceCm * MidDistanceCm)
                {
                    Tier = 1;
                }
            }

            const int32 MyIndex = TierSeen[Tier]++;
            if (bTierExhausted[Tier] || MyIndex < TierCursor[Tier])
            {
                continue;
            }
            if (TierBudget[Tier] <= 0)
            {
                bTierExhausted[Tier] = true;
                NextCursor[Tier] = MyIndex;
                continue;
            }

            TraceSegment(Transforms[Index], Velocity[Index], Motion[Index], Data, Life[Index], Presentation[Index],
                Motion[Index].LastCheckLocation, End, false);
            Motion[Index].LastCheckLocation = End;
            --TierBudget[Tier];

            if (!Life[Index].bPendingDestroy && Life[Index].FuseRemainingSeconds > 0.0f
                && Life[Index].FuseRemainingSeconds <= Delta)
            {
                Life[Index].bPendingDestroy = true;
            }
        }
    });

    for (int32 Tier = 0; Tier < 3; ++Tier)
    {
        if (bTierExhausted[Tier])
        {
            TierCursor[Tier] = NextCursor[Tier];
            continue;
        }
        TierCursor[Tier] = 0;
    }

    if (ImpactChannel.IsValid())
    {
        FBBBProjectilePresentation::PublishImpacts(*World, *ImpactChannel.Get(), Impacts);
    }
}

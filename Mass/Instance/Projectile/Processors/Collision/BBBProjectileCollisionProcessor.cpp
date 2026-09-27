#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Processors/Collision/BBBProjectileCollisionProcessor.h"

#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "Kismet/GameplayStatics.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Movement/BBBProjectileMotionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Collision/BBBProjectileCollisionFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Projectile/Fragments/Lifetime/BBBProjectileLifetimeFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/LocalControl/Health/FBBBMonsterDamageLocalControlPacket.h"

UBBBProjectileCollisionProcessor::UBBBProjectileCollisionProcessor()
    : EntityQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PostPhysics;
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
}

void UBBBProjectileCollisionProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    UBBBMassSubsystem* Mass = World->GetSubsystem<UBBBMassSubsystem>();
    EntityQuery.ForEachEntityChunk(Context, [World, Mass](FMassExecutionContext& Chunk)
    {
        auto Transforms = Chunk.GetMutableFragmentView<FTransformFragment>();
        const auto Motion = Chunk.GetFragmentView<FBBBProjectileMotionFragment>();
        const auto Velocity = Chunk.GetFragmentView<FMassVelocityFragment>();
        auto Collision = Chunk.GetMutableFragmentView<FBBBProjectileCollisionFragment>();
        auto Life = Chunk.GetMutableFragmentView<FBBBProjectileLifetimeFragment>();
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
                const bool bEntityHit = Mass->TraceEntities(Start, End, Data.CollisionRadiusCm, IgnoredEntities, Target, EntityTime);
                const bool bUseEntity = bEntityHit && (!bWorldHit || EntityTime < WorldHit.Time);
                if (!bUseEntity && !bWorldHit)
                {
                    break;
                }

                const FVector HitPosition = bUseEntity ? FMath::Lerp(Start, End, EntityTime) : WorldHit.Location;
                if (bUseEntity)
                {
                    if (Data.bCanCauseDamage)
                    {
                        FBBBMonsterDamageLocalControlPacket Packet;
                        Packet.Damage = Data.Damage;
                        Mass->SubmitInput(Target, MoveTemp(Packet));
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
}

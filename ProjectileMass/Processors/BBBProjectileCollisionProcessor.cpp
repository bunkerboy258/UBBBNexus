#include "BBBWork/UBBBNexus/ProjectileMass/Processors/BBBProjectileCollisionProcessor.h"

#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileRuntimeFragment.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Entity/BBBProjectileTag.h"
#include "BBBWork/UBBBNexus/ProjectileMass/Processors/BBBProjectileMovementProcessor.h"

UBBBProjectileCollisionProcessor::UBBBProjectileCollisionProcessor()
    : ProjectileQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PostPhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteAfter.Add(UBBBProjectileMovementProcessor::StaticClass()->GetFName());
}

void UBBBProjectileCollisionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    ProjectileQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    ProjectileQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
    ProjectileQuery.AddRequirement<FBBBProjectileRuntimeFragment>(EMassFragmentAccess::ReadWrite);
    ProjectileQuery.AddTagRequirement<FBBBProjectileTag>(EMassFragmentPresence::All);
}

void UBBBProjectileCollisionProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();

    if (!ensureMsgf(World != nullptr, TEXT("[ProjectileMass] 碰撞处理器无法获取所属世界")))
    {
        return;
    }

    ProjectileQuery.ForEachEntityChunk(Context, [World](FMassExecutionContext& ChunkContext)
    {
        TArrayView<FTransformFragment> Transforms = ChunkContext.GetMutableFragmentView<FTransformFragment>();
        const TConstArrayView<FMassVelocityFragment> Velocities = ChunkContext.GetFragmentView<FMassVelocityFragment>();
        TArrayView<FBBBProjectileRuntimeFragment> RuntimeFragments = ChunkContext.GetMutableFragmentView<FBBBProjectileRuntimeFragment>();

        for (int32 Index = 0; Index < ChunkContext.GetNumEntities(); ++Index)
        {
            FBBBProjectileRuntimeFragment& Runtime = RuntimeFragments[Index];

            if (Runtime.bPendingDestroy)
            {
                continue;
            }

            const FVector Direction = Velocities[Index].Value.GetSafeNormal();

            if (Direction.IsNearlyZero())
            {
                Runtime.bPendingDestroy = true;
                continue;
            }

            const FVector TraceEnd = Transforms[Index].GetTransform().GetLocation();
            FVector TraceStart = Runtime.PreviousLocation;

            if (FVector::DistSquared(TraceStart, TraceEnd) <= SMALL_NUMBER)
            {
                continue;
            }

            FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(BBBProjectileMassSweep), false);
            QueryParams.AddIgnoredActor(Runtime.DamageCauser.Get());
            QueryParams.AddIgnoredActor(Runtime.InstigatorPawn.Get());
            QueryParams.AddIgnoredActor(Runtime.LastHitActor.Get());

            const float Radius = FMath::Max(Runtime.CollisionRadiusCm, 0.0f);
            const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(Radius);

            while (FVector::DistSquared(TraceStart, TraceEnd) > SMALL_NUMBER)
            {
                FHitResult Hit;
                bool bHit = false;

                if (Radius > KINDA_SMALL_NUMBER)
                {
                    bHit = World->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, Runtime.CollisionChannel, CollisionShape, QueryParams);
                }

                if (Radius <= KINDA_SMALL_NUMBER)
                {
                    bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, Runtime.CollisionChannel, QueryParams);
                }

                if (!bHit)
                {
                    break;
                }

                AActor* HitActor = Hit.GetActor();

                if (!IsValid(HitActor))
                {
                    Runtime.bPendingDestroy = true;
                    Transforms[Index].GetMutableTransform().SetLocation(Hit.Location);
                    break;
                }

                UGameplayStatics::ApplyPointDamage(
                    HitActor,
                    Runtime.Damage,
                    Direction,
                    Hit,
                    Runtime.EventInstigator.Get(),
                    Runtime.DamageCauser.Get(),
                    nullptr);

                if (Runtime.RemainingPenetrations <= 0)
                {
                    Runtime.bPendingDestroy = true;
                    Transforms[Index].GetMutableTransform().SetLocation(Hit.Location);
                    break;
                }

                --Runtime.RemainingPenetrations;
                Runtime.Damage *= Runtime.PenetrationDamageMultiplier;
                Runtime.LastHitActor = HitActor;
                QueryParams.AddIgnoredActor(HitActor);
                TraceStart = Hit.Location + Direction * FMath::Max(Radius + 1.0f, 1.0f);
            }
        }
    });
}

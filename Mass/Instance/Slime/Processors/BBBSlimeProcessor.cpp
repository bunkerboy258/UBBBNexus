#include "BBBSlimeProcessor.h"
#include "GameFramework/Pawn.h"

#include "MassExecutionContext.h"
#include "MassEntitySubsystem.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Fragments/BBBSlimeInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Presentation/BBBSlimePresentationActor.h"

UBBBSlimeProcessor::UBBBSlimeProcessor() : SlimeQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::AllNetModes);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Movement;
    ExecutionOrder.ExecuteAfter.Add(BBBMassProcessingGroups::Parse);
}

void UBBBSlimeProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    SlimeQuery.AddRequirement<FBBBSlimeFragment>(EMassFragmentAccess::ReadWrite);
    SlimeQuery.AddRequirement<FBBBSlimeInputFragment>(EMassFragmentAccess::ReadWrite);
}

void UBBBSlimeProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    if (!World)
    {
        return;
    }
    const bool bHost = World->GetNetMode() != NM_Client;
    const float Now = World->GetTimeSeconds();
    const float Delta = FMath::Clamp(Context.GetDeltaTimeSeconds(), 0.0f, 0.25f);
    SlimeQuery.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
    {
        auto States = Chunk.GetMutableFragmentView<FBBBSlimeFragment>();
        auto Inputs = Chunk.GetMutableFragmentView<FBBBSlimeInputFragment>();
        for (int32 I = 0; I < States.Num(); ++I)
        {
            auto& State = States[I];
            auto& Fact = State.Fact;
            auto& Spawn = Inputs[I].Spawn;
            auto& Remote = Inputs[I].State;
            if (Remote.bActive)
            {
                Remote.bActive = false;
                if (!bHost && Remote.Packet.IsValid() && Remote.Packet.CanApply(State))
                {
                    Remote.Packet.Apply(State);
                }
            }
            if (Spawn.bActive)
            {
                Spawn.bActive = false;
                if (bHost && Spawn.Packet.IsValid() && Spawn.Packet.CanApply(State))
                {
                    Spawn.Packet.Apply(State);
                }
            }
            if (!Fact.IsValid() || Now >= Fact.EndsAt)
            {
                if (State.Presentation.IsValid())
                {
                    State.Presentation->Destroy();
                }
                if (bHost)
                {
                    Chunk.Defer().DestroyEntity(Chunk.GetEntity(I));
                }
                continue;
            }
            if (bHost && Fact.Phase == 0 && Delta > 0)
            {
                FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBSlimeFlight), false);
                Params.AddIgnoredActor(State.SourceActor.Get());
                FVector Velocity = Fact.Velocity;
                Velocity.Z += World->GetGravityZ() * Fact.Definition->GravityScale * Delta;
                const FVector End = Fact.Location + (Fact.Velocity + Velocity) * (Delta * 0.5f);
                FHitResult Hit;
                if (World->SweepSingleByChannel(Hit, Fact.Location, End, FQuat::Identity, ECC_Pawn,
                    FCollisionShape::MakeSphere(Fact.Definition->FlightRadius), Params))
                {
                    FHitResult Floor = Hit;
                    const bool bPawnHit = Cast<APawn>(Hit.GetActor()) != nullptr;
                    if (bPawnHit || Hit.ImpactNormal.Z < 0.6f)
                    {
                        if (bPawnHit)
                        {
                            Params.AddIgnoredActor(Hit.GetActor());
                        }
                        const FVector From = Hit.ImpactPoint + Hit.ImpactNormal * 15;
                        if (!World->LineTraceSingleByChannel(Floor, From, From - FVector::UpVector * 250, ECC_WorldStatic, Params)
                            || Floor.ImpactNormal.Z < 0.6f)
                        {
                            Fact.EndsAt = Now;
                            continue;
                        }
                    }
                    Fact.Location = Floor.ImpactPoint;
                    Fact.Normal = Floor.ImpactNormal.GetSafeNormal();
                    Fact.Velocity = FVector::ZeroVector;
                    Fact.Phase = 1;
                    Fact.EndsAt = Now + Fact.Definition->Lifetime;
                    UE_LOG(LogTemp, Verbose, TEXT("[BBBSlime] 落地 Id=%s"), *Fact.InstanceId.ToString());
                }
                if (!Hit.bBlockingHit)
                {
                    Fact.Location = End;
                    Fact.Velocity = Velocity;
                }
                ++Fact.Revision;
            }
            if (World->GetNetMode() == NM_DedicatedServer)
            {
                continue;
            }
            if (!State.Presentation.IsValid())
            {
                State.Presentation = World->SpawnActor<ABBBSlimePresentationActor>();
            }
            if (State.Presentation.IsValid())
            {
                State.Presentation->ApplyFact(Fact);
            }
        }
    });
}

int32 UBBBSlimeProcessor::Count(UWorld& World, const FGuid& SourceId, int32& SourceCount)
{
    SourceCount = 0;
    int32 Total = 0;
    auto* Entities = World.GetSubsystem<UMassEntitySubsystem>();
    if (!Entities || !IsInGameThread())
    {
        return 0;
    }
    auto& Manager = Entities->GetMutableEntityManager();
    FMassEntityQuery Query(Manager.AsShared());
    Query.AddRequirement<FBBBSlimeFragment>(EMassFragmentAccess::ReadOnly);
    Query.AddRequirement<FBBBSlimeInputFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
    FMassExecutionContext Context(Manager, 0, false);
    Query.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
    {
        const auto States = Chunk.GetFragmentView<FBBBSlimeFragment>();
        const auto Inputs = Chunk.GetFragmentView<FBBBSlimeInputFragment>();
        for (int32 Index = 0; Index < States.Num(); ++Index)
        {
            const auto& State = States[Index];
            const FBBBSlimeFact* Fact = &State.Fact;
            if (!Fact->InstanceId.IsValid() && !Inputs.IsEmpty() && Inputs[Index].Spawn.bActive)
            {
                Fact = &Inputs[Index].Spawn.Packet.Fact;
            }
            if (Fact->InstanceId.IsValid() && Fact->EndsAt <= World.GetTimeSeconds())
            {
                continue;
            }
            ++Total;
            SourceCount += Fact->SourceId == SourceId ? 1 : 0;
        }
    });
    return Total;
}

bool UBBBSlimeProcessor::QueryExposure(UWorld& World, const FVector& Position, const float Radius,
    float& DamagePerSecond, float& SpeedScale)
{
    DamagePerSecond = 0;
    SpeedScale = 1;
    bool bContact = false;
    auto* Entities = World.GetSubsystem<UMassEntitySubsystem>();
    if (!Entities || !IsInGameThread() || Position.ContainsNaN() || !FMath::IsFinite(Radius) || Radius < 0)
    {
        return false;
    }
    auto& Manager = Entities->GetMutableEntityManager();
    FMassEntityQuery Query(Manager.AsShared());
    Query.AddRequirement<FBBBSlimeFragment>(EMassFragmentAccess::ReadOnly);
    FMassExecutionContext Context(Manager, 0, false);
    Query.ForEachEntityChunk(Context, [&](FMassExecutionContext& Chunk)
    {
        for (const auto& State : Chunk.GetFragmentView<FBBBSlimeFragment>())
        {
            const auto& Fact = State.Fact;
            if (Fact.Phase != 1 || !Fact.Definition || World.GetTimeSeconds() >= Fact.EndsAt)
            {
                continue;
            }
            const FVector Offset = Position - Fact.Location;
            const float Height = FVector::DotProduct(Offset, Fact.Normal);
            const FVector Tangent = Offset - Fact.Normal * Height;
            if (Height < -15 || Height > 60 || Tangent.SizeSquared() > FMath::Square(Fact.Definition->Radius + Radius))
            {
                continue;
            }
            FHitResult Hit;
            FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBSlimeContact), false);
            FCollisionObjectQueryParams WorldGeometry;
            WorldGeometry.AddObjectTypesToQuery(ECC_WorldStatic);
            WorldGeometry.AddObjectTypesToQuery(ECC_WorldDynamic);
            if (World.LineTraceSingleByObjectType(Hit, Fact.Location + Fact.Normal * 8,
                Position + Fact.Normal * 8, WorldGeometry, Params))
            {
                continue;
            }
            bContact = true;
            DamagePerSecond = FMath::Max(DamagePerSecond, Fact.Definition->DamagePerSecond);
            SpeedScale = FMath::Min(SpeedScale, Fact.Definition->SpeedScale);
        }
    });
    return bContact;
}

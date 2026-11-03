#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/Processors/BBBWorldDropProcessor.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/DomainData/Context/BBBWorldDropUpdateContext.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Item/BBBItemDefinition.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
void FBBBWorldDropProcessor::Update(FBBBWorldDropUpdateContext &Context) const
{
    auto &Drop = Context.Drop;
    if (!Context.bCausal || !Drop.GetWorld()->IsGameWorld())
    {
        return;
    }
    auto &State = Context.Data.Drop.DropState;
    auto &Snapshot = State.Snapshot;
    for (const auto &Initialization : State.Initializations)
    {
        if (!Snapshot.bClaimed && Snapshot.Operation == 0 && !Snapshot.bConsumed
            && IsValid(Initialization.Definition) && !Initialization.Definition->ItemId.IsNone()
            && Initialization.Quantity > 0 && Initialization.Quantity <= 1000000
            && !Initialization.Velocity.ContainsNaN())
        {
            Snapshot.Definition = Initialization.Definition;
            Snapshot.Quantity = Initialization.Quantity;
            Snapshot.bLanded = false;
            ++Snapshot.Revision;
            Drop.Body->SetSimulatePhysics(true);
            Drop.Body->SetPhysicsLinearVelocity(Initialization.Velocity);
            Drop.Body->WakeAllRigidBodies();
        }
    }
    State.Initializations.Reset();
    if (!Snapshot.bLanded && Drop.Body->GetPhysicsLinearVelocity().SizeSquared() < FMath::Square(10.0f))
    {
        FHitResult Hit;
        FCollisionQueryParams Query(SCENE_QUERY_STAT(BBBWorldDropGround), false, &Drop);
        const float Radius = Drop.Body->GetScaledSphereRadius();
        if (Drop.GetWorld()->LineTraceSingleByChannel(Hit, Drop.GetActorLocation(),
            Drop.GetActorLocation() - FVector(0, 0, Radius + 5.0f), ECC_Visibility, Query)
            && Hit.ImpactNormal.Z > 0.2f)
        {
            Snapshot.bLanded = true;
            ++Snapshot.Revision;
            Drop.Body->PutAllRigidBodiesToSleep();
        }
    }
    for (const auto &Completion : State.Completions)
    {
        const auto &Receipt = Completion.Receipt;
        if (!Snapshot.bClaimed || Snapshot.bConsumed || Completion.Character != Snapshot.Recipient
            || Receipt.Grant.Drop != &Drop || Receipt.Grant.DropId != Snapshot.DropId
            || Receipt.Grant.Operation != Snapshot.Operation)
        {
            continue;
        }
        Snapshot.bConsumed = Receipt.bSucceeded;
        Snapshot.bClaimed = false;
        Snapshot.Recipient = nullptr;
        ++Snapshot.Revision;
    }
    State.Completions.Reset();
    if (Snapshot.bClaimed && Snapshot.Recipient && !IsValid(Snapshot.Recipient))
    {
        UE_LOG(LogTemp, Warning, TEXT("世界掉落物交付目标已失效 保留待确认交付以防重复发放 Drop=%s Operation=%llu"),
            *Snapshot.DropId.ToString(), Snapshot.Operation);
        Snapshot.Recipient = nullptr;
        ++Snapshot.Revision;
    }
    for (const auto &Claim : State.Claims)
    {
        if (!Drop.IsAvailable() || Snapshot.Definition->ItemId.IsNone()
            || !IsValid(Claim.Character) || Claim.DropId != Snapshot.DropId
            || Claim.Character->GetWorld() != Drop.GetWorld()
            || Claim.Character->GetLifePhase() != EBBBCharacterLifePhase::Alive
            || !Claim.Character->IsPlayerControlled())
        {
            continue;
        }
        Snapshot.Recipient = Claim.Character;
        Snapshot.bClaimed = true;
        ++Snapshot.Operation;
        ++Snapshot.Revision;
        break;
    }
    State.Claims.Reset();
}

#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/Processors/BBBCharacterPickupProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/DomainData/Context/BBBCharacterPickupUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Config/Pickup/BBBCharacterPickupConfig.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBCharacterItemAddLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
void FBBBCharacterPickupProcessor::Update(FBBBCharacterPickupUpdateContext &Context) const
{
    auto &State = Context.Data.Pickup.PickupState;
    State.Requests.Reset();
    State.Receipts.Reset();
    for (auto It = State.Records.CreateIterator(); It; ++It)
    {
        if (!IsValid(It.Key()))
        {
            It.RemoveCurrent();
        }
    }
    const double Now = Context.Data.External.ReadWorldState().WorldTimeSeconds;
    const auto &Results = Context.Data.Item.ReadCharacterItemOperationState().AcquisitionResults;
    bool bAcquiredDrop = false;
    for (auto &Entry : State.Records)
    {
        auto &Record = Entry.Value;
        const auto &Grant = Record.Receipt.Grant;
        for (const auto &Result : Results)
        {
            if (Record.bAwaitingResult && Grant.DropId == Result.Key.SourceId && Grant.Operation == Result.Key.Operation)
            {
                Record.bAwaitingResult = false;
                Record.Receipt.bSucceeded = Result.bSucceeded;
                State.Receipts.Add(Record.Receipt);
                State.NextProbeAt = Now;
                bAcquiredDrop |= Result.bSucceeded;
                if (!Result.bSucceeded)
                {
                    Record.RetryAt = Now + Context.Config.RejectedRetryInterval;
                }
            }
        }
    }
    if (bAcquiredDrop && Context.Config.PickupSound && Context.Character.IsLocallyControlled()
        && Context.Character.IsPlayerControlled())
    {
        UGameplayStatics::PlaySound2D(&Context.Character, Context.Config.PickupSound);
    }
    for (const auto &Grant : State.PendingGrants)
    {
        if (!IsValid(Grant.Drop) || Grant.Drop->GetWorld() != Context.Character.GetWorld())
        {
            continue;
        }
        auto *Existing = State.Records.Find(Grant.Drop);
        if (Existing && Existing->Receipt.Grant.DropId == Grant.DropId
            && Grant.Operation <= Existing->Receipt.Grant.Operation)
        {
            if (Grant.Operation == Existing->Receipt.Grant.Operation && !Existing->bAwaitingResult)
            {
                State.Receipts.Add(Existing->Receipt);
            }
            continue;
        }
        FBBBCharacterPickupRecord Record;
        Record.Receipt.Grant = Grant;
        if (Context.Data.Life.ReadLifeState().Phase != EBBBCharacterLifePhase::Alive)
        {
            State.Receipts.Add(Record.Receipt);
            State.Records.Add(Grant.Drop, Record);
            continue;
        }
        Record.bAwaitingResult = true;
        State.Records.Add(Grant.Drop, Record);
        FBBBCharacterItemAddLocalControlPacket Packet;
        Packet.ItemIds.Add(Grant.ItemId);
        Packet.Quantities.Add(Grant.Quantity);
        Packet.Keys.Add(FBBBCharacterItemAcquisitionKey{Grant.DropId, Grant.Operation});
        Context.Character.SubmitInput(MoveTemp(Packet));
    }
    State.PendingGrants.Reset();
    bool bAwaiting = false;
    for (const auto &Entry : State.Records)
    {
        bAwaiting |= Entry.Value.bAwaitingResult;
    }
    if (!bAwaiting && Now >= State.NextProbeAt && Context.Character.IsPlayerControlled()
        && Context.Data.Life.ReadLifeState().Phase == EBBBCharacterLifePhase::Alive)
    {
        State.NextProbeAt = Now + FMath::Max(0.05f, Context.Config.ProbeInterval);
        const auto *Capsule = Context.Character.GetCapsuleComponent();
        const FVector Origin = Capsule->GetComponentLocation();
        const double AxisHalfLength = Capsule->GetScaledCapsuleHalfHeight_WithoutHemisphere();
        const float Radius = FMath::Max(1.0f, Context.Config.Distance);
        FCollisionQueryParams Query(SCENE_QUERY_STAT(BBBAutoPickup), false, &Context.Character);
        TArray<AActor *> Attached;
        Context.Character.GetAttachedActors(Attached, true, true);
        Query.AddIgnoredActors(Attached);
        TArray<FOverlapResult> Overlaps;
        Context.Character.GetWorld()->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity,
            FCollisionObjectQueryParams(ECC_WorldDynamic),
            FCollisionShape::MakeCapsule(Radius, Radius + AxisHalfLength), Query);
        ABBBWorldDropItem *Nearest = nullptr;
        double NearestDistance = FMath::Square(static_cast<double>(Radius));
        for (const auto &Overlap : Overlaps)
        {
            auto *Drop = Cast<ABBBWorldDropItem>(Overlap.GetActor());
            if (!IsValid(Drop) || !Drop->IsAvailable())
            {
                continue;
            }
            const auto *Record = State.Records.Find(Drop);
            if (Record && Record->Receipt.Grant.DropId == Drop->ReadDropSnapshot().DropId
                && (Record->Receipt.bSucceeded || Now < Record->RetryAt))
            {
                continue;
            }
            const FVector DropLocation = Drop->GetActorLocation();
            FVector ReachOrigin = Origin;
            ReachOrigin.Z += FMath::Clamp(DropLocation.Z - Origin.Z, -AxisHalfLength, AxisHalfLength);
            const double Distance = FVector::DistSquared(ReachOrigin, DropLocation);
            if (Distance > NearestDistance)
            {
                continue;
            }
            FHitResult Hit;
            FCollisionQueryParams SightQuery = Query;
            SightQuery.AddIgnoredActor(Drop);
            if (Context.Character.GetWorld()->LineTraceSingleByChannel(Hit, Origin,
                DropLocation, ECC_Visibility, SightQuery))
            {
                continue;
            }
            Nearest = Drop;
            NearestDistance = Distance;
        }
        if (Nearest)
        {
            FBBBCharacterPickupRequest Request;
            Request.Drop = Nearest;
            Request.DropId = Nearest->ReadDropSnapshot().DropId;
            State.Requests.Add(Request);
        }
    }
    if (!State.Requests.IsEmpty() || !State.Receipts.IsEmpty())
    {
        ++State.DeliverySerial;
    }
}
void FBBBCharacterPickupProcessor::Shutdown(FBBBCharacterPickupUpdateContext &Context) const
{
    auto &State = Context.Data.Pickup.PickupState;
    State.PendingGrants.Reset();
    State.Requests.Reset();
    State.Receipts.Reset();
    State.Records.Reset();
}

#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/BehaviorSystem/Processors/BBBMaodieTestBehaviorProcessor.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Input/LocalControl/Initialize/FBBBWorldDropInitializeLocalControlPacket.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
void FBBBMaodieTestBehaviorProcessor::Update(ABBBMaodieTest &Actor, float DeltaSeconds, bool bCausal)
{
    if (!bCausal)
    {
        return;
    }
    auto &State = Actor.RuntimeData.Behavior.State;
    if (!State.bEnabled)
    {
        return;
    }
    const auto &Config = Actor.GetTestConfig();
    const double Now = Actor.GetWorld()->GetTimeSeconds();
    auto *Controller = Cast<AAIController>(Actor.GetController());
    APawn *Visible = nullptr;
    float BestDistance = FMath::Square(Config.DetectionRadius);
    if (State.ThrownCount < Config.ThrowLimit)
    {
        for (auto It = Actor.GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            APawn *Pawn = It->Get() ? It->Get()->GetPawn() : nullptr;
            if (!IsValid(Pawn) || Pawn == &Actor)
            {
                continue;
            }
            const float Distance = FVector::DistSquared(Pawn->GetActorLocation(), Actor.GetActorLocation());
            if (Distance > BestDistance)
            {
                continue;
            }
            FHitResult Hit;
            FCollisionQueryParams Query(SCENE_QUERY_STAT(MaodieSight), false, &Actor);
            const bool bBlocked = Actor.GetWorld()->LineTraceSingleByChannel(Hit,
                Actor.GetActorLocation() + FVector(0.0f, 0.0f, 20.0f), Pawn->GetActorLocation(), ECC_Visibility, Query);
            if (!bBlocked || Hit.GetActor() == Pawn)
            {
                BestDistance = Distance;
                Visible = Pawn;
            }
        }
    }
    if (Visible)
    {
        if (State.Target.Get() != Visible)
        {
            State.Target = Visible;
            State.NextActionAt = Now + Config.ObserveSeconds;
        }
        State.LastSeenAt = Now;
    }
    if (!Visible && (Now - State.LastSeenAt >= Config.LoseInterestDelay || State.ThrownCount >= Config.ThrowLimit))
    {
        State.Target.Reset();
    }
    if (State.Snapshot.Phase == 3 && Now < State.HissEndsAt)
    {
        return;
    }
    if (auto *Target = State.Target.Get())
    {
        if (Controller)
        {
            Controller->StopMovement();
        }
        Actor.GetCharacterMovement()->bOrientRotationToMovement = false;
        FVector Direction = Target->GetActorLocation() - Actor.GetActorLocation();
        Direction.Z = 0.0f;
        Actor.SetActorRotation(FMath::RInterpTo(Actor.GetActorRotation(), Direction.Rotation(), DeltaSeconds, Config.TurnSpeed));
        State.Snapshot.Phase = 2;
        if (Visible && Now >= State.NextActionAt)
        {
            State.Snapshot.Phase = 3;
            ++State.Snapshot.HissSerial;
            State.HissEndsAt = Now + Config.HissDuration;
            State.NextActionAt = Now + FMath::Max(Config.ThrowInterval, Config.HissDuration);
            const FVector Forward = Direction.GetSafeNormal();
            const FVector Origin = Actor.GetActorLocation() + Forward * 55.0f + FVector(0.0f, 0.0f, 35.0f);
            FActorSpawnParameters Params;
            Params.Owner = &Actor;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
            auto *Drop = Actor.GetWorld()->SpawnActor<ABBBWorldDropItem>(Origin, FRotator::ZeroRotator, Params);
            if (Drop)
            {
                FBBBWorldDropInitialization Initialization;
                Initialization.Definition = Config.DropPool[FMath::RandHelper(Config.DropPool.Num())];
                Initialization.Quantity = 1;
                Initialization.Velocity = Forward * Config.ThrowSpeed + FVector(0.0f, 0.0f, Config.ThrowUpSpeed);
                FBBBWorldDropInitializeLocalControlPacket Packet;
                Packet.Initializations.Add(Initialization);
                Drop->SubmitInput(MoveTemp(Packet));
                ++State.ThrownCount;
            }
            if (!Drop)
            {
                UE_LOG(LogTemp, Warning, TEXT("耄耋投掷生成被阻挡 未消耗次数 %s"), *Actor.GetPathName());
            }
        }
        return;
    }
    Actor.GetCharacterMovement()->bOrientRotationToMovement = true;
    State.Snapshot.Phase = Actor.GetVelocity().SizeSquared2D() > 25.0f ? 1 : 0;
    if (Controller && Controller->GetMoveStatus() == EPathFollowingStatus::Moving)
    {
        State.NextActionAt = Now + Config.PauseSeconds;
        return;
    }
    if (Now < State.NextActionAt)
    {
        return;
    }
    State.NextActionAt = Now + Config.PauseSeconds;
    auto *Navigation = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Actor.GetWorld());
    FNavLocation Destination;
    if (Controller && Navigation && Navigation->GetRandomReachablePointInRadius(State.Home, Config.WanderRadius, Destination))
    {
        Controller->MoveToLocation(Destination.Location, 20.0f);
    }
}

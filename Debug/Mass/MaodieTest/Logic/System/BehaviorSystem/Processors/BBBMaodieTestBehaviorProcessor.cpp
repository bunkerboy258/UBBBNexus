#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/BehaviorSystem/Processors/BBBMaodieTestBehaviorProcessor.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/BBBWorldDropItem.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Input/LocalControl/Initialize/FBBBWorldDropInitializeLocalControlPacket.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
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
    State.Snapshot.bHasLookTarget = false;
    State.Snapshot.LookTarget = FVector::ZeroVector;
    const auto LookAtPlayer = [&](APawn &Player, bool bTurnBody)
    {
        State.Snapshot.bHasLookTarget = true;
        State.Snapshot.LookTarget = Player.GetPawnViewLocation();
        if (bTurnBody)
        {
            const FVector LocalDirection = Actor.GetActorQuat().UnrotateVector(Player.GetActorLocation() - Actor.GetActorLocation());
            if (FMath::Abs(LocalDirection.Rotation().Yaw) > Config.HeadYawLimit)
            {
                const float Yaw = (Player.GetActorLocation() - Actor.GetActorLocation()).Rotation().Yaw;
                Actor.SetActorRotation(FMath::RInterpTo(Actor.GetActorRotation(), FRotator(0.0f, Yaw, 0.0f), DeltaSeconds, Config.TurnSpeed));
            }
        }
    };
    APawn *Visible = nullptr;
    float BestDistance = FMath::Square(Config.DetectionRadius);
    for (auto It = Actor.GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APawn *Pawn = It->Get() ? It->Get()->GetPawn() : nullptr;
        APlayerState *Player = Pawn ? Pawn->GetPlayerState() : nullptr;
        if (!IsValid(Pawn) || Pawn == &Actor || !IsValid(Player)
            || State.PlayerThrowCounts.FindRef(Player) >= Config.ThrowLimit)
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
    if (Visible)
    {
        if (State.Target.Get() != Visible)
        {
            State.Target = Visible;
            State.NextThrowAt = Now + Config.ObserveSeconds;
            State.Snapshot.Phase = 2;
        }
        State.LastSeenAt = Now;
    }
    APawn *RememberedTarget = State.Target.Get();
    const bool bExhausted = RememberedTarget
        && State.PlayerThrowCounts.FindRef(RememberedTarget->GetPlayerState()) >= Config.ThrowLimit;
    if (!Visible && (bExhausted || Now - State.LastSeenAt >= Config.LoseInterestDelay))
    {
        State.Target.Reset();
    }
    if (auto *Target = State.Target.Get())
    {
        if (Controller)
        {
            Controller->StopMovement();
        }
        Actor.GetCharacterMovement()->MaxWalkSpeed = Config.MoveSpeed;
        State.bFollowing = false;
        State.NextFollowRequestAt = 0.0;
        Actor.GetCharacterMovement()->bOrientRotationToMovement = false;
        LookAtPlayer(*Target, true);
        FVector Direction = Target->GetActorLocation() - Actor.GetActorLocation();
        Direction.Z = 0.0f;
        const bool bContinueThrowing = Visible && State.Snapshot.Phase == 3;
        State.Snapshot.Phase = bContinueThrowing ? 3 : 2;
        if (Visible && Now >= State.NextThrowAt)
        {
            State.NextThrowAt = Now + FMath::FRandRange(1.0f - Config.IntervalJitter, 1.0f + Config.IntervalJitter) / Config.ThrowFrequency;
            const FVector Forward = Direction.GetSafeNormal();
            const FVector Origin = Actor.GetActorLocation() + Forward * 55.0f + FVector(0.0f, 0.0f, FMath::FRandRange(Config.ThrowHeightMin, Config.ThrowHeightMax));
            FActorSpawnParameters Params;
            Params.Owner = &Actor;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
            auto *Drop = Actor.GetWorld()->SpawnActor<ABBBWorldDropItem>(Origin, FRotator::ZeroRotator, Params);
            if (Drop)
            {
                FBBBWorldDropInitialization Initialization;
                Initialization.Definition = Config.DropPool[FMath::RandHelper(Config.DropPool.Num())];
                Initialization.Quantity = 1;
                Initialization.Velocity = Forward * FMath::FRandRange(Config.ThrowSpeedMin, Config.ThrowSpeedMax)
                    + FVector(0.0f, 0.0f, FMath::FRandRange(Config.ThrowUpSpeedMin, Config.ThrowUpSpeedMax));
                FBBBWorldDropInitializeLocalControlPacket Packet;
                Packet.Initializations.Add(Initialization);
                Drop->SubmitInput(MoveTemp(Packet));
                if (!bContinueThrowing || Now >= State.HissEndsAt)
                {
                    ++State.Snapshot.HissSerial;
                    State.HissEndsAt = Now + Config.HissDuration;
                }
                State.Snapshot.Phase = 3;
                const int32 Count = ++State.PlayerThrowCounts.FindOrAdd(Target->GetPlayerState());
                if (Count >= Config.ThrowLimit)
                {
                    State.FollowPlayer = Target->GetPlayerState();
                }
                UE_LOG(LogTemp, Display, TEXT("MaodieThrow Player=%s Count=%d Limit=%d Time=%.3f Velocity=%s Height=%.2f NextInterval=%.3f"),
                    *Target->GetPlayerState()->GetPlayerName(), Count, Config.ThrowLimit, Now,
                    *Initialization.Velocity.ToCompactString(), Origin.Z - Actor.GetActorLocation().Z, State.NextThrowAt - Now);
            }
            if (!Drop)
            {
                UE_LOG(LogTemp, Warning, TEXT("耄耋投掷生成被阻挡 未消耗次数 %s"), *Actor.GetPathName());
            }
        }
        return;
    }
    APawn *FollowTarget = nullptr;
    if (State.FollowPlayer.IsValid())
    {
        for (auto It = Actor.GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController *Player = It->Get();
            if (Player && Player->PlayerState == State.FollowPlayer.Get() && IsValid(Player->GetPawn()))
            {
                FollowTarget = Player->GetPawn();
                break;
            }
        }
    }
    if (FollowTarget)
    {
        const float Distance = FVector::Dist2D(Actor.GetActorLocation(), FollowTarget->GetActorLocation());
        const bool bWasFollowing = State.bFollowing;
        if (Distance > Config.FollowStartDistance)
        {
            State.bFollowing = true;
        }
        if (Distance <= Config.FollowStopDistance + 5.0f)
        {
            State.bFollowing = false;
        }
        if (bWasFollowing != State.bFollowing)
        {
            UE_LOG(LogTemp, Display, TEXT("MaodieFollow Moving=%d Distance=%.1f Player=%s"),
                State.bFollowing, Distance, *State.FollowPlayer->GetPlayerName());
        }
        Actor.GetCharacterMovement()->MaxWalkSpeed = Config.FollowSpeed;
        Actor.GetCharacterMovement()->bOrientRotationToMovement = State.bFollowing;
        LookAtPlayer(*FollowTarget, !State.bFollowing);
        if (Controller && State.bFollowing && Now >= State.NextFollowRequestAt)
        {
            FAIMoveRequest Request(FollowTarget);
            Request.SetAcceptanceRadius(Config.FollowStopDistance);
            Request.SetReachTestIncludesAgentRadius(false);
            Request.SetReachTestIncludesGoalRadius(false);
            Controller->MoveTo(Request);
            State.NextFollowRequestAt = Now + 0.5;
        }
        if (Controller && !State.bFollowing)
        {
            Controller->StopMovement();
        }
        State.Snapshot.Phase = Actor.GetVelocity().SizeSquared2D() > 25.0f ? 1 : 0;
        return;
    }
    State.bFollowing = false;
    Actor.GetCharacterMovement()->MaxWalkSpeed = Config.MoveSpeed;
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

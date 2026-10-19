#include "BBBMonsterDirectorPlacementProcessor.h"
#include "BBBMonsterDirectorSpawnInitializationProcessor.h"
#include "MassExecutionContext.h"
#include "MassEntitySubsystem.h"
#include "MassCommonFragments.h"
#include "MassCommands.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "NavigationData.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnPoolDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnData.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Presentation/BBBMonsterDirectorPresentationActor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterDirectorTag.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"

UBBBMonsterDirectorPlacementProcessor::UBBBMonsterDirectorPlacementProcessor()
    : DirectorQuery(*this), MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Decision;
    ExecutionOrder.ExecuteAfter.Add(TEXT("BBBMonsterDirectorScheduleProcessor"));
}

void UBBBMonsterDirectorPlacementProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    DirectorQuery.AddTagRequirement<FBBBMonsterDirectorTag>(EMassFragmentPresence::All);
    DirectorQuery.AddRequirement<FBBBMonsterDirectorFragment>(EMassFragmentAccess::ReadWrite);
    DirectorQuery.AddRequirement<FBBBMonsterDirectorSpawnFragment>(EMassFragmentAccess::ReadWrite);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterGroundFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMovementFragment>(EMassFragmentAccess::ReadOnly);
}

bool UBBBMonsterDirectorPlacementProcessor::QueryRegions(UWorld& World, const FMassEntityHandle Entity, const int32 StageIndex,
    TArray<FBBBMonsterSpawnRegionDefinition>& Result)
{
    Result.Reset();
    const auto* Entities = World.GetSubsystem<UMassEntitySubsystem>();
    if (!Entities || !Entities->GetEntityManager().IsEntityValid(Entity))
    {
        return false;
    }
    const auto* State = Entities->GetEntityManager().GetFragmentDataPtr<FBBBMonsterDirectorFragment>(Entity);
    const auto* Definition = State ? State->Definition.Get() : nullptr;
    if (!Definition || !Definition->Stages.IsValidIndex(StageIndex))
    {
        return false;
    }
    for (const auto& Region : State->Regions)
    {
        if (Definition->Stages[StageIndex].RegionIds.Contains(Region.Id))
        {
            Result.Add(Region);
        }
    }
    return !Result.IsEmpty();
}

void UBBBMonsterDirectorPlacementProcessor::Filter(UWorld& World, const UBBBMonsterDirectorDefinition& Definition, const int32 StageIndex,
    const TArray<FBBBMonsterSpawnRegionDefinition>& Regions, const TArray<FVector>& Candidates,
    const TArray<FVector>& ObserverPositions, const TArray<FVector>& ObserverDirections, const TArray<FVector>& Occupied,
    const int32 Count, TArray<FVector>& Result, TArray<FVector>& Rejected, TArray<FString>& Reasons)
{
    Result.Reset();
    Rejected.Reset();
    Reasons.Reset();
    if (!Definition.Stages.IsValidIndex(StageIndex) || Count <= 0)
    {
        return;
    }
    const auto* Pool = Definition.Stages[StageIndex].Pool.Get();
    float Radius = 1.0f;
    float HalfHeight = 1.0f;
    if (Pool)
    {
        for (const auto& Entry : Pool->Entries)
        {
            if (Entry.Definition && Entry.Weight > 0.0f)
            {
                Radius = FMath::Max(Radius, Entry.Definition->CollisionRadius);
                HalfHeight = FMath::Max(HalfHeight, Entry.Definition->CapsuleHalfHeight);
            }
        }
    }
    const float Spacing = FMath::Max(Definition.MinSpacing, Radius * 2.0f);
    auto* Navigation = FNavigationSystem::GetCurrent<UNavigationSystemV1>(&World);
    const ANavigationData* NavData = Navigation ? Navigation->GetDefaultNavDataInstance(FNavigationSystem::DontCreate) : nullptr;
    for (const FVector& Candidate : Candidates)
    {
        if (Result.Num() >= Count)
        {
            break;
        }
        FString Reason;
        FVector Location = Candidate;
        FNavLocation Projected;
        if (Candidate.ContainsNaN())
        {
            Reason = TEXT("候选位置无效");
        }
        if (Reason.IsEmpty() && (!NavData || !Navigation->ProjectPointToNavigation(Candidate, Projected, FVector(100.0, 100.0, 600.0), NavData)))
        {
            Reason = TEXT("没有可用导航位置");
        }
        if (Reason.IsEmpty())
        {
            Location = Projected.Location;
            bool bInside = false;
            for (const auto& Region : Regions)
            {
                bInside |= Region.Contains(Location);
            }
            if (!bInside)
            {
                Reason = TEXT("导航投影越出生成区域");
            }
        }
        if (Reason.IsEmpty() && ObserverPositions.IsEmpty())
        {
            Reason = TEXT("没有有效玩家或预览参考点");
        }
        if (Reason.IsEmpty())
        {
            for (const FVector& Observer : ObserverPositions)
            {
                if (FVector::DistSquared(Location, Observer) < FMath::Square(Definition.MinPlayerDistance))
                {
                    Reason = TEXT("距离玩家过近");
                    break;
                }
            }
        }
        const FVector Center = Location + FVector(0.0, 0.0, HalfHeight);
        FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMonsterDirectorPlacement), false);
        if (Reason.IsEmpty() && World.OverlapBlockingTestByChannel(Center, FQuat::Identity, ECC_Pawn,
            FCollisionShape::MakeCapsule(Radius, HalfHeight), Params))
        {
            Reason = TEXT("胶囊空间被占用");
        }
        if (Reason.IsEmpty())
        {
            for (const FVector& Other : Occupied)
            {
                if (FVector::DistSquared(Location, Other) < FMath::Square(Spacing))
                {
                    Reason = TEXT("位置已被小怪或其他批次占用");
                    break;
                }
            }
            for (const FVector& Other : Result)
            {
                if (FVector::DistSquared(Location, Other) < FMath::Square(Spacing))
                {
                    Reason = TEXT("同批出生间距不足");
                    break;
                }
            }
        }
        if (Reason.IsEmpty() && Definition.bAvoidVisible)
        {
            for (int32 Index = 0; Index < ObserverPositions.Num(); ++Index)
            {
                const FVector Direction = (Center - ObserverPositions[Index]).GetSafeNormal();
                if (!ObserverDirections.IsValidIndex(Index) || FVector::DotProduct(Direction, ObserverDirections[Index]) < 0.5f)
                {
                    continue;
                }
                FHitResult Hit;
                if (!World.LineTraceSingleByChannel(Hit, ObserverPositions[Index], Center, ECC_Visibility, Params))
                {
                    Reason = TEXT("出生位置位于玩家可见范围");
                    break;
                }
            }
        }
        if (Reason.IsEmpty() && Definition.bRequirePath)
        {
            bool bReachable = false;
            for (const FVector& Observer : ObserverPositions)
            {
                FNavLocation Goal;
                if (!Navigation->ProjectPointToNavigation(Observer, Goal, FVector(150.0, 150.0, 600.0), NavData))
                {
                    continue;
                }
                FPathFindingQuery Query(nullptr, *NavData, Location, Goal.Location, NavData->GetDefaultQueryFilter());
                Query.SetAllowPartialPaths(false);
                const FPathFindingResult Path = Navigation->FindPathSync(Query);
                if (Path.IsSuccessful() && Path.Path.IsValid() && Path.Path->IsValid() && !Path.Path->IsPartial() &&
                    Path.Path->GetPathPoints().Num() >= 2 &&
                    FVector::DistSquared(Path.Path->GetPathPoints().Last().Location, Goal.Location) <= FMath::Square(15.0f))
                {
                    bReachable = true;
                    break;
                }
            }
            if (!bReachable)
            {
                Reason = TEXT("与玩家没有完整可达路径");
            }
        }
        if (Reason.IsEmpty())
        {
            Result.Add(Location);
            continue;
        }
        Rejected.Add(Candidate);
        Reasons.Add(Reason);
    }
}

void UBBBMonsterDirectorPlacementProcessor::Execute(FMassEntityManager&, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    UEnvQueryManager* QueryManager = UEnvQueryManager::GetCurrent(World);
    TArray<FVector> Observers;
    TArray<FVector> Directions;
    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* Controller = It->Get();
        if (Controller && Controller->GetPawn() && Controller->GetPawn()->CanBeDamaged())
        {
            FVector Position;
            FRotator Rotation;
            Controller->GetPlayerViewPoint(Position, Rotation);
            Observers.Add(Position);
            Directions.Add(Rotation.Vector());
        }
    }
    TArray<FVector> Occupied;
    MonsterQuery.ForEachEntityChunk(Context, [&Occupied](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Ground = Chunk.GetFragmentView<FBBBMonsterGroundFragment>();
        const auto Movement = Chunk.GetFragmentView<FBBBMonsterMovementFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            if (Health[Index].CurrentHealth > 0.0f)
            {
                const float Height = Ground[Index].CapsuleHalfHeight > 0.0f ? Ground[Index].CapsuleHalfHeight : Movement[Index].CapsuleHalfHeight;
                Occupied.Add(Transforms[Index].GetTransform().GetLocation() - FVector(0.0, 0.0, Height));
            }
        }
    });
    DirectorQuery.ForEachEntityChunk(Context, [&Occupied](FMassExecutionContext& Chunk)
    {
        const auto Spawns = Chunk.GetFragmentView<FBBBMonsterDirectorSpawnFragment>();
        for (const auto& Spawn : Spawns)
        {
            if (Spawn.bCreating)
            {
                Occupied.Append(Spawn.Locations);
            }
        }
    });
    DirectorQuery.ForEachEntityChunk(Context, [World, QueryManager, &Observers, &Directions, &Occupied](FMassExecutionContext& Chunk)
    {
        auto States = Chunk.GetMutableFragmentView<FBBBMonsterDirectorFragment>();
        auto Spawns = Chunk.GetMutableFragmentView<FBBBMonsterDirectorSpawnFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& State = States[Index];
            auto& Spawn = Spawns[Index];
            if (Spawn.Reserved == 0 && Spawn.QueryId != INDEX_NONE)
            {
                if (QueryManager)
                {
                    QueryManager->AbortQuery(Spawn.QueryId);
                }
                Spawn.QueryId = INDEX_NONE;
            }
            const auto* Definition = State.Definition.Get();
            auto* Owner = State.Presentation.Get();
            if (!Definition || !Owner || State.Phase != EBBBMonsterDirectorPhase::Spawning || Spawn.Reserved == 0)
            {
                continue;
            }
            const FMassEntityHandle Entity = Chunk.GetEntity(Index);
            if (Spawn.bNeedsQuery)
            {
                Spawn.bNeedsQuery = false;
                FBBBMonsterDirectorQueryResultLocalControlPacket Packet;
                Packet.InstanceId = State.InstanceId;
                Packet.Generation = State.Generation;
                Packet.StageIndex = State.StageIndex;
                Packet.RequestId = Spawn.RequestId;
                if (!QueryManager)
                {
                    World->GetSubsystem<UBBBMassSubsystem>()->SubmitInput(Entity, MoveTemp(Packet));
                    continue;
                }
                const TWeakObjectPtr<UWorld> WeakWorld = World;
                FQueryFinishedSignature Finished = FQueryFinishedSignature::CreateLambda([WeakWorld, Entity, Packet](TSharedPtr<FEnvQueryResult> Result) mutable
                {
                    if (!WeakWorld.IsValid())
                    {
                        return;
                    }
                    if (Result.IsValid() && Result->IsSuccessful())
                    {
                        Result->GetAllAsLocations(Packet.Locations);
                    }
                    WeakWorld->GetSubsystem<UBBBMassSubsystem>()->SubmitInput(Entity, MoveTemp(Packet));
                });
                FEnvQueryRequest Request(Definition->PlacementQuery, Owner);
                Request.SetFloatParam(TEXT("DirectorStage"), State.StageIndex);
                Request.SetFloatParam(TEXT("DirectorSeed"), HashCombine(GetTypeHash(State.InstanceId), Spawn.RequestId) & 0x00ffffffu);
                Spawn.QueryId = Request.Execute(EEnvQueryRunMode::AllMatching, Finished);
                if (Spawn.QueryId == INDEX_NONE)
                {
                    World->GetSubsystem<UBBBMassSubsystem>()->SubmitInput(Entity, MoveTemp(Packet));
                }
                continue;
            }
            if (!Spawn.bResultReady || Spawn.bCreating)
            {
                continue;
            }
            TArray<FBBBMonsterSpawnRegionDefinition> Regions;
            QueryRegions(*World, Entity, State.StageIndex, Regions);
            TArray<FVector> Selected;
            TArray<FVector> Rejected;
            TArray<FString> Reasons;
            Filter(*World, *Definition, State.StageIndex, Regions, Spawn.Locations, Observers, Directions, Occupied,
                Spawn.Reserved, Selected, Rejected, Reasons);
            Spawn.bResultReady = false;
            Spawn.bCreating = true;
            Spawn.Locations = Selected;
            Occupied.Append(Selected);
            FBBBMonsterDirectorCreatedLocalControlPacket Packet;
            Packet.InstanceId = State.InstanceId;
            Packet.Generation = State.Generation;
            Packet.StageIndex = State.StageIndex;
            Packet.RequestId = Spawn.RequestId;
            Packet.Reason = Reasons.IsEmpty() ? TEXT("没有合格候选点") : Reasons[0];
            const TWeakObjectPtr<UWorld> WeakWorld = World;
            const TWeakObjectPtr<ABBBMonsterDirectorPresentationActor> WeakOwner = Owner;
            Chunk.Defer().PushCommand<FMassDeferredCreateCommand>([WeakWorld, WeakOwner, Entity, Selected, Packet](FMassEntityManager& Manager) mutable
            {
                UWorld* CurrentWorld = WeakWorld.Get();
                if (!CurrentWorld || !WeakOwner.IsValid() || !Manager.IsEntityValid(Entity))
                {
                    return;
                }
                const auto* Current = Manager.GetFragmentDataPtr<FBBBMonsterDirectorFragment>(Entity);
                const auto* Pending = Manager.GetFragmentDataPtr<FBBBMonsterDirectorSpawnFragment>(Entity);
                if (!Current || !Pending || !Packet.CanApply(*Current, *Pending))
                {
                    return;
                }
                const auto* Settings = Current->Definition.Get();
                const auto* Pool = Settings->Stages[Packet.StageIndex].Pool.Get();
                FRandomStream Random(HashCombine(GetTypeHash(Packet.InstanceId), Packet.RequestId));
                double TotalWeight = 0.0;
                for (const auto& Entry : Pool->Entries)
                {
                    TotalWeight += Entry.Weight;
                }
                TMap<const UBBBMonsterDefinition*, FBBBMonsterDirectorSpawnData> Batches;
                for (const FVector& Location : Selected)
                {
                    const double Choice = Random.FRand() * TotalWeight;
                    double Sum = 0.0;
                    for (const auto& Entry : Pool->Entries)
                    {
                        Sum += Entry.Weight;
                        if (Choice < Sum)
                        {
                            auto& Batch = Batches.FindOrAdd(Entry.Definition);
                            Batch.DirectorId = Packet.InstanceId;
                            Batch.Generation = Packet.Generation;
                            Batch.StageIndex = Packet.StageIndex;
                            Batch.Transforms.Add(FTransform(FQuat::Identity, Location + FVector(0.0, 0.0, Entry.Definition->CapsuleHalfHeight)));
                            break;
                        }
                    }
                }
                auto* Mass = CurrentWorld->GetSubsystem<UBBBMassSubsystem>();
                for (const auto& Batch : Batches)
                {
                    TArray<FMassEntityHandle> Created;
                    Mass->CreateEntities(*Batch.Key->EntityConfig, Batch.Value.Transforms.Num(), FConstStructView::Make(Batch.Value),
                        UBBBMonsterDirectorSpawnInitializationProcessor::StaticClass(), Created);
                    Packet.Count += Created.Num();
                }
                Packet.Time = CurrentWorld->GetTimeSeconds();
                if (Packet.Count > 0)
                {
                    Packet.Reason = FString::Printf(TEXT("实际生成 %d 只"), Packet.Count);
                }
                UE_LOG(LogTemp, Display, TEXT("[BBBMonsterDirector] Instance=%s Stage=%d Request=%u Created=%d Reason=%s"),
                    *Packet.InstanceId.ToString(), Packet.StageIndex, Packet.RequestId, Packet.Count, *Packet.Reason);
                Mass->SubmitInput(Entity, MoveTemp(Packet));
            });
        }
    });
}

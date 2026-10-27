#include "BBBMonsterSpawnProcessor.h"
#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MassCommands.h"
#include "HAL/PlatformTime.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "NavigationPath.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassSubsystem.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassProcessingGroups.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnPoolDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterSpawnInputFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterSpawnData.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Health/BBBMonsterHealthFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterMovementFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterGroundFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Processors/Spawn/BBBMonsterSpawnInitializationProcessor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Tags/BBBMonsterTag.h"

namespace
{
    void ReadObservers(UWorld& World, TArray<FVector>& Observers, TArray<FVector>& Directions)
    {
        for (auto It = World.GetPlayerControllerIterator(); It; ++It)
        {
            auto* Controller = It->Get();
            if (Controller && Controller->GetPawn() && Controller->GetPawn()->CanBeDamaged())
            {
                FVector Position;
                FRotator Rotation;
                Controller->GetPlayerViewPoint(Position, Rotation);
                Observers.Add(Position);
                Directions.Add(Rotation.Vector());
            }
        }
    }

    void ReadLivePositions(FMassEntityManager& Manager, TArray<FVector>& Occupied)
    {
        FMassEntityQuery Query(Manager.AsShared());
        Query.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
        Query.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
        Query.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
        Query.AddRequirement<FBBBMonsterMovementFragment>(EMassFragmentAccess::ReadOnly);
        Query.AddRequirement<FBBBMonsterGroundFragment>(EMassFragmentAccess::ReadOnly);
        /** 延迟创建期间只读遍历 不在查询析构时嵌套冲刷当前创建命令 */
        FMassExecutionContext Context(Manager, 0.0f, false);
        Query.ForEachEntityChunk(Context, [&Occupied](FMassExecutionContext& Chunk)
        {
            const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
            const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
            const auto Movement = Chunk.GetFragmentView<FBBBMonsterMovementFragment>();
            const auto Ground = Chunk.GetFragmentView<FBBBMonsterGroundFragment>();
            for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
            {
                if (Health[Index].CurrentHealth > 0.0f)
                {
                    const float HalfHeight = Ground[Index].CapsuleHalfHeight > 0.0f
                        ? Ground[Index].CapsuleHalfHeight : Movement[Index].CapsuleHalfHeight;
                    Occupied.Add(Transforms[Index].GetTransform().GetLocation() - FVector::UpVector * HalfHeight);
                }
            }
        });
    }

    bool CanCreateNow(UWorld& World, const UBBBMonsterSpawnDefinition& Definition, const UBBBMonsterDefinition& Monster,
        const FTransform& Transform, const TArray<FBBBMonsterSpawnRegionDefinition>& Regions,
        const TArray<FVector>& Observers, const TArray<FVector>& Directions, const TArray<FVector>& Occupied)
    {
        const FVector Center = Transform.GetLocation();
        const FVector Feet = Center - FVector::UpVector * (Monster.CapsuleHalfHeight + 2.0f);
        if (Observers.IsEmpty() || Observers.Num() > Definition.MaxObservers || !Regions.ContainsByPredicate([&Feet](const auto& Region)
        {
            return Region.Contains(Feet);
        }))
        {
            return false;
        }
        FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMonsterSpawnCommit), false);
        if (World.OverlapBlockingTestByChannel(Center, FQuat::Identity, ECC_Pawn,
            FCollisionShape::MakeCapsule(Monster.CollisionRadius, Monster.CapsuleHalfHeight), Params))
        {
            return false;
        }
        FHitResult Floor;
        if (!World.LineTraceSingleByChannel(Floor, Feet + FVector::UpVector * 10.0f,
            Feet - FVector::UpVector * 15.0f, ECC_Pawn, Params)
            || Floor.ImpactNormal.Z < FMath::Cos(FMath::DegreesToRadians(Monster.MaxWalkableSlopeAngle)))
        {
            return false;
        }
        const float Spacing = FMath::Max(Definition.MinSpacing, Monster.CollisionRadius * 2.0f);
        if (Occupied.ContainsByPredicate([&Feet, Spacing](const FVector& Other)
        {
            return FVector::DistSquared(Feet, Other) < FMath::Square(Spacing);
        }))
        {
            return false;
        }
        for (int32 Index = 0; Index < Observers.Num(); ++Index)
        {
            if (FVector::DistSquared(Feet, Observers[Index]) < FMath::Square(Definition.MinPlayerDistance))
            {
                return false;
            }
            FHitResult Blocker;
            if (Definition.bAvoidVisible && Directions.IsValidIndex(Index)
                && FVector::DotProduct((Center - Observers[Index]).GetSafeNormal(), Directions[Index]) >= 0.5f
                && !World.LineTraceSingleByChannel(Blocker, Observers[Index], Center, ECC_Visibility, Params))
            {
                return false;
            }
        }
        return true;
    }
}

UBBBMonsterSpawnProcessor::UBBBMonsterSpawnProcessor()
    : SpawnQuery(*this), MonsterQuery(*this)
{
    bAutoRegisterWithProcessingPhases = true;
    bRequiresGameThreadExecution = true;
    ProcessingPhase = EMassProcessingPhase::PrePhysics;
    ExecutionFlags = static_cast<uint8>(EProcessorExecutionFlags::Server | EProcessorExecutionFlags::Standalone);
    ExecutionOrder.ExecuteInGroup = BBBMassProcessingGroups::Decision;
    ExecutionOrder.ExecuteAfter.Add(TEXT("BBBMonsterDirectorScheduleProcessor"));
}

uint32 UBBBMonsterSpawnProcessor::RandomBits(uint32 Seed, const uint32 Index, const uint32 Channel)
{
    Seed ^= Index * 0x9e3779b9u ^ Channel;
    Seed ^= Seed >> 16;
    Seed *= 0x7feb352du;
    Seed ^= Seed >> 15;
    Seed *= 0x846ca68bu;
    return (Seed ^ (Seed >> 16)) | 1u;
}

FVector UBBBMonsterSpawnProcessor::Candidate(const TArray<FBBBMonsterSpawnRegionDefinition>& Regions, const uint32 Seed, const int32 Index)
{
    if (Regions.IsEmpty())
    {
        return FVector::ZeroVector;
    }
    const auto& Region = Regions[Index % Regions.Num()];
    FRandomStream Random(RandomBits(Seed, Index, 0x104u));
    FVector Local = FVector::ZeroVector;
    if (Region.Shape == EBBBMonsterSpawnRegionShape::Box)
    {
        Local = FVector(Random.FRandRange(-Region.Extent.X, Region.Extent.X), Random.FRandRange(-Region.Extent.Y, Region.Extent.Y), 0.0);
    }
    if (Region.Shape != EBBBMonsterSpawnRegionShape::Box)
    {
        const float Angle = Random.FRand() * UE_TWO_PI;
        const float Radius = FMath::Sqrt(Random.FRand()) * Region.Extent.X;
        Local = FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.0);
    }
    return Region.Transform.TransformPosition(Local);
}

UBBBMonsterDefinition* UBBBMonsterSpawnProcessor::SelectDefinition(const UBBBMonsterSpawnDefinition& Definition, const uint32 Seed, const int32 Index)
{
    if (!Definition.Pool)
    {
        return nullptr;
    }
    double Total = 0.0;
    for (const auto& Entry : Definition.Pool->Entries)
    {
        if (Entry.Definition && Entry.Weight > 0.0f)
        {
            Total += Entry.Weight;
        }
    }
    FRandomStream Random(RandomBits(Seed, Index, 0x205u));
    const double Choice = Random.FRand() * Total;
    double Sum = 0.0;
    for (const auto& Entry : Definition.Pool->Entries)
    {
        if (!Entry.Definition || Entry.Weight <= 0.0f)
        {
            continue;
        }
        Sum += Entry.Weight;
        if (Choice < Sum)
        {
            return Entry.Definition;
        }
    }
    return nullptr;
}

void UBBBMonsterSpawnProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>&)
{
    SpawnQuery.AddRequirement<FBBBMonsterSpawnFragment>(EMassFragmentAccess::ReadWrite);
    SpawnQuery.AddRequirement<FBBBMonsterSpawnInputFragment>(EMassFragmentAccess::ReadWrite);
    SpawnQuery.AddRequirement<FBBBMonsterDirectorFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
    MonsterQuery.AddTagRequirement<FBBBMonsterTag>(EMassFragmentPresence::All);
    MonsterQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterHealthFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterMovementFragment>(EMassFragmentAccess::ReadOnly);
    MonsterQuery.AddRequirement<FBBBMonsterGroundFragment>(EMassFragmentAccess::ReadOnly);
}

void UBBBMonsterSpawnProcessor::Filter(UWorld& World, const UBBBMonsterSpawnDefinition& Definition,
    const TArray<FBBBMonsterSpawnRegionDefinition>& Regions, const TArray<FVector>& Candidates,
    const TArray<FVector>& Observers, const TArray<FVector>& Directions, const TArray<FVector>& Occupied,
    const int32 Count, TArray<FVector>& Result, TArray<FVector>& Rejected, TArray<FString>& Reasons)
{
    Result.Reset();
    Rejected.Reset();
    Reasons.Reset();
    float Radius = 1.0f;
    float Height = 1.0f;
    if (Definition.Pool)
    {
        for (const auto& Entry : Definition.Pool->Entries)
        {
            if (Entry.Definition && Entry.Weight > 0.0f)
            {
                Radius = FMath::Max(Radius, Entry.Definition->CollisionRadius);
                Height = FMath::Max(Height, Entry.Definition->CapsuleHalfHeight);
            }
        }
    }
    auto* Navigation = FNavigationSystem::GetCurrent<UNavigationSystemV1>(&World);
    const auto* NavData = Navigation ? Navigation->GetDefaultNavDataInstance(FNavigationSystem::DontCreate) : nullptr;
    const float SpacingSquared = FMath::Square(FMath::Max(Definition.MinSpacing, Radius * 2.0f));
    FCollisionQueryParams Params(SCENE_QUERY_STAT(BBBMonsterSpawn), false);
    for (const auto& Input : Candidates)
    {
        if (Result.Num() >= Count)
        {
            break;
        }
        FString Reason;
        FVector Location = Input;
        FNavLocation Projected;
        if (Input.ContainsNaN() || !NavData || !Navigation->ProjectPointToNavigation(Input, Projected, FVector(100.0, 100.0, 600.0), NavData))
        {
            Reason = TEXT("没有可用导航位置");
        }
        if (Reason.IsEmpty())
        {
            FHitResult Ground;
            const FVector Offset(0.0, 0.0, Definition.GroundTraceDistance);
            if (!World.LineTraceSingleByChannel(Ground, Projected.Location + Offset, Projected.Location - Offset, ECC_WorldStatic, Params)
                || Ground.ImpactNormal.Z < 0.5f)
            {
                Reason = TEXT("没有可站立地面");
            }
            Location = Ground.bBlockingHit ? Ground.ImpactPoint : Projected.Location;
        }
        if (Reason.IsEmpty())
        {
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
        if (Reason.IsEmpty() && (Observers.Num() > Definition.MaxObservers || Observers.IsEmpty()))
        {
            Reason = TEXT("有效玩家数量不在检查预算内");
        }
        for (const auto& Observer : Observers)
        {
            if (Reason.IsEmpty() && FVector::DistSquared(Location, Observer) < FMath::Square(Definition.MinPlayerDistance))
            {
                Reason = TEXT("距离玩家过近");
            }
        }
        const FVector Center = Location + FVector(0.0, 0.0, Height + 2.0f);
        if (Reason.IsEmpty() && World.OverlapBlockingTestByChannel(Center, FQuat::Identity, ECC_Pawn,
            FCollisionShape::MakeCapsule(Radius, Height), Params))
        {
            Reason = TEXT("胶囊空间被占用");
        }
        for (const auto& Other : Occupied)
        {
            if (Reason.IsEmpty() && FVector::DistSquared(Location, Other) < SpacingSquared)
            {
                Reason = TEXT("位置已被小怪或其他批次占用");
            }
        }
        for (const auto& Other : Result)
        {
            if (Reason.IsEmpty() && FVector::DistSquared(Location, Other) < SpacingSquared)
            {
                Reason = TEXT("同批出生间距不足");
            }
        }
        if (Reason.IsEmpty() && Definition.bAvoidVisible)
        {
            for (int32 Index = 0; Index < Observers.Num(); ++Index)
            {
                if (!Directions.IsValidIndex(Index) || FVector::DotProduct((Center - Observers[Index]).GetSafeNormal(), Directions[Index]) < 0.5f)
                {
                    continue;
                }
                FHitResult Hit;
                if (!World.LineTraceSingleByChannel(Hit, Observers[Index], Center, ECC_Visibility, Params))
                {
                    Reason = TEXT("出生位置位于玩家可见范围");
                    break;
                }
            }
        }
        if (Reason.IsEmpty() && Definition.bRequirePath)
        {
            bool bReachable = false;
            for (const auto& Observer : Observers)
            {
                FNavLocation Goal;
                if (!Navigation->ProjectPointToNavigation(Observer, Goal, FVector(150.0, 150.0, 600.0), NavData))
                {
                    continue;
                }
                FPathFindingQuery Query(nullptr, *NavData, Location, Goal.Location, NavData->GetDefaultQueryFilter());
                Query.SetAllowPartialPaths(false);
                const auto Path = Navigation->FindPathSync(Query);
                if (Path.IsSuccessful() && Path.Path.IsValid() && Path.Path->IsValid() && !Path.Path->IsPartial()
                    && !Path.Path->GetPathPoints().IsEmpty()
                    && FVector::DistSquared(Path.Path->GetPathPoints().Last().Location, Goal.Location) <= FMath::Square(15.0f))
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
        Rejected.Add(Input);
        Reasons.Add(Reason);
    }
}

void UBBBMonsterSpawnProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    UWorld* World = Context.GetWorld();
    TArray<FVector> Observers;
    TArray<FVector> Directions;
    ReadObservers(*World, Observers, Directions);
    TArray<FVector> Occupied;
    MonsterQuery.ForEachEntityChunk(Context, [&Occupied](FMassExecutionContext& Chunk)
    {
        const auto Transforms = Chunk.GetFragmentView<FTransformFragment>();
        const auto Health = Chunk.GetFragmentView<FBBBMonsterHealthFragment>();
        const auto Movement = Chunk.GetFragmentView<FBBBMonsterMovementFragment>();
        const auto Ground = Chunk.GetFragmentView<FBBBMonsterGroundFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            if (Health[Index].CurrentHealth > 0.0f)
            {
                const float HalfHeight = Ground[Index].CapsuleHalfHeight > 0.0f
                    ? Ground[Index].CapsuleHalfHeight : Movement[Index].CapsuleHalfHeight;
                Occupied.Add(Transforms[Index].GetTransform().GetLocation() - FVector::UpVector * HalfHeight);
            }
        }
    });
    int32 CandidateBudget = 64;
    int32 CreateBudget = 64;
    const int32 WorldAlive = Occupied.Num();
    int32 WorldLimit = MAX_int32;
    int32 PendingPoints = 0;
    float TimeBudgetMs = 16.0f;
    TArray<FMassEntityHandle> SpawnOrder;
    SpawnQuery.ForEachEntityChunk(Context, [&Occupied, &CandidateBudget, &CreateBudget,
        &WorldLimit, &PendingPoints, &TimeBudgetMs, &SpawnOrder](FMassExecutionContext& Chunk)
    {
        auto States = Chunk.GetMutableFragmentView<FBBBMonsterSpawnFragment>();
        auto Inputs = Chunk.GetMutableFragmentView<FBBBMonsterSpawnInputFragment>();
        const auto Directors = Chunk.GetFragmentView<FBBBMonsterDirectorFragment>();
        for (int32 Index = 0; Index < Chunk.GetNumEntities(); ++Index)
        {
            auto& State = States[Index];
            auto& Input = Inputs[Index];
            const auto Consume = [&State](auto& Slot)
            {
                if (!Slot.bActive)
                {
                    return;
                }
                Slot.bActive = false;
                if (Slot.Packet.IsValid() && Slot.Packet.CanApply(State))
                {
                    Slot.Packet.Apply(State);
                }
            };
            Consume(Input.Request);
            Consume(Input.Resume);
            Consume(Input.Pause);
            Consume(Input.Stop);
            if (!Directors.IsEmpty() && Directors[Index].Definition.IsValid()
                && Directors[Index].Phase != EBBBMonsterDirectorPhase::Inactive
                && Directors[Index].Phase != EBBBMonsterDirectorPhase::Stopped
                && Directors[Index].Phase != EBBBMonsterDirectorPhase::Completed)
            {
                WorldLimit = FMath::Min(WorldLimit, Directors[Index].Definition->WorldMaxAlive);
            }
            if (State.bActive && State.Definition.IsValid())
            {
                WorldLimit = FMath::Min(WorldLimit, State.Definition->WorldMaxAlive);
                PendingPoints += State.Points.Num();
                for (const auto& Point : State.Points)
                {
                    if (const auto* Monster = Point.Definition.Get())
                    {
                        Occupied.Add(Point.Transform.GetLocation() - FVector::UpVector * Monster->CapsuleHalfHeight);
                    }
                }
            }
            if (State.bActive && !State.bPaused)
            {
                if (const auto* Definition = State.Definition.Get())
                {
                    CandidateBudget = FMath::Min(CandidateBudget, Definition->CandidatesPerFrame);
                    CreateBudget = FMath::Min(CreateBudget, Definition->CreatesPerFrame);
                    TimeBudgetMs = FMath::Min(TimeBudgetMs, Definition->TimeBudgetMs);
                }
                SpawnOrder.Add(Chunk.GetEntity(Index));
            }
        }
    });
    int32 SelectionSlots = FMath::Max(0, WorldLimit - WorldAlive - PendingPoints);
    CreateBudget = FMath::Min(CreateBudget, FMath::Max(0, WorldLimit - WorldAlive));
    const double StartedAt = FPlatformTime::Seconds();
    const int32 FirstJob = SpawnOrder.IsEmpty() ? 0 : NextSpawnIndex % SpawnOrder.Num();
    for (int32 Offset = 0; Offset < SpawnOrder.Num(); ++Offset)
    {
        const int32 JobIndex = (FirstJob + Offset) % SpawnOrder.Num();
        const FMassEntityHandle Entity = SpawnOrder[JobIndex];
        auto& State = EntityManager.GetFragmentDataChecked<FBBBMonsterSpawnFragment>(Entity);
        const auto* Definition = State.Definition.Get();
        if (!State.bActive || State.bPaused || State.bCreating || !Definition)
        {
            continue;
        }
        while (CandidateBudget > 0 && SelectionSlots > 0 && State.Accepted < State.Requested
            && State.Cursor < Definition->CandidateCount
            && (FPlatformTime::Seconds() - StartedAt) * 1000.0 < TimeBudgetMs)
        {
            --CandidateBudget;
            const int32 CandidateIndex = State.Cursor++;
            const FVector Location = Candidate(State.Regions, State.Seed, CandidateIndex);
            TArray<FVector> Selected;
            TArray<FVector> Rejected;
            TArray<FString> Reasons;
            Filter(*World, *Definition, State.Regions, {Location}, Observers, Directions, Occupied, 1, Selected, Rejected, Reasons);
            if (Selected.IsEmpty())
            {
                ++State.Rejections.FindOrAdd(Reasons.IsEmpty() ? TEXT("没有合格候选点") : Reasons[0]);
                continue;
            }
            auto* Monster = SelectDefinition(*Definition, State.Seed, CandidateIndex);
            if (!Monster)
            {
                ++State.Rejections.FindOrAdd(TEXT("模板权重无效"));
                continue;
            }
            FRandomStream Heading(RandomBits(State.Seed, CandidateIndex, 0x306u));
            const float Yaw = Definition->bRandomYaw ? Heading.FRandRange(-180.0f, 180.0f) : Definition->FixedYaw;
            auto& Point = State.Points.AddDefaulted_GetRef();
            Point.Definition = Monster;
            Point.Seed = RandomBits(State.Seed, CandidateIndex, 0x407u);
            Point.Transform = FTransform(FRotator(0.0f, Yaw, 0.0f), Selected[0] + FVector(0.0, 0.0, Monster->CapsuleHalfHeight + 2.0f));
            Occupied.Add(Selected[0]);
            ++State.Accepted;
            --SelectionSlots;
        }
        const bool bSelectionDone = State.Accepted >= State.Requested || State.Cursor >= Definition->CandidateCount;
        if (State.Points.IsEmpty() && bSelectionDone)
        {
            State.bActive = false;
            State.Reason = FString::Printf(TEXT("实际生成 %d 只 检查 %d 点"), State.Created, State.Cursor);
            for (const auto& Reason : State.Rejections)
            {
                State.Reason += FString::Printf(TEXT(" %s:%d"), *Reason.Key, Reason.Value);
            }
            continue;
        }
        if (CreateBudget == 0 || State.Points.IsEmpty())
        {
            if (SelectionSlots == 0)
            {
                State.Reason = TEXT("等待共享世界存活额度");
            }
            continue;
        }
        const int32 Count = FMath::Min(CreateBudget, State.Points.Num());
        CreateBudget -= Count;
        TArray<FBBBMonsterSpawnPoint> Points;
        Points.Append(State.Points.GetData(), Count);
        State.Points.RemoveAt(0, Count);
        State.bCreating = true;
        const FGuid OwnerId = State.OwnerId;
        const uint32 RequestId = State.RequestId;
        const uint32 Generation = State.Generation;
        const int32 StageIndex = State.StageIndex;
        const TWeakObjectPtr<UWorld> WeakWorld = World;
        Context.Defer().PushCommand<FMassDeferredCreateCommand>([WeakWorld, Entity, OwnerId, RequestId, Generation, StageIndex, Points, WorldLimit](FMassEntityManager& Manager)
        {
            UWorld* CurrentWorld = WeakWorld.Get();
            if (!CurrentWorld || !Manager.IsEntityValid(Entity))
            {
                return;
            }
            auto* Current = Manager.GetFragmentDataPtr<FBBBMonsterSpawnFragment>(Entity);
            if (!Current || Current->OwnerId != OwnerId || Current->RequestId != RequestId)
            {
                return;
            }
            if (!Current->bActive || Current->bPaused)
            {
                Current->bCreating = false;
                Current->Points.Insert(Points.GetData(), Points.Num(), 0);
                return;
            }
            int32 CreatedCount = 0;
            int32 RejectedCount = 0;
            TArray<FVector> CurrentObservers;
            TArray<FVector> CurrentDirections;
            TArray<FVector> CurrentOccupied;
            ReadObservers(*CurrentWorld, CurrentObservers, CurrentDirections);
            ReadLivePositions(Manager, CurrentOccupied);
            int32 Capacity = FMath::Max(0, WorldLimit - CurrentOccupied.Num());
            const auto* SpawnDefinition = Current->Definition.Get();
            if (!SpawnDefinition)
            {
                Current->Accepted = FMath::Max(Current->Created, Current->Accepted - Points.Num());
                Current->bCreating = false;
                return;
            }
            TMap<UBBBMonsterDefinition*, FBBBMonsterSpawnData> Batches;
            for (const auto& Point : Points)
            {
                auto* Monster = Point.Definition.Get();
                if (!Monster || !Monster->EntityConfig || !CanCreateNow(*CurrentWorld, *SpawnDefinition, *Monster,
                    Point.Transform, Current->Regions, CurrentObservers, CurrentDirections, CurrentOccupied))
                {
                    ++RejectedCount;
                    continue;
                }
                if (Capacity <= 0)
                {
                    Current->Points.Add(Point);
                    continue;
                }
                --Capacity;
                CurrentOccupied.Add(Point.Transform.GetLocation() - FVector::UpVector * Monster->CapsuleHalfHeight);
                auto& Batch = Batches.FindOrAdd(Monster);
                Batch.DirectorId = OwnerId;
                Batch.Generation = Generation;
                Batch.StageIndex = StageIndex;
                Batch.Transforms.Add(Point.Transform);
                Batch.Seeds.Add(Point.Seed);
            }
            for (const auto& Batch : Batches)
            {
                TArray<FMassEntityHandle> Created;
                CurrentWorld->GetSubsystem<UBBBMassSubsystem>()->CreateEntities(*Batch.Key->EntityConfig,
                    Batch.Value.Transforms.Num(), FConstStructView::Make(Batch.Value),
                    UBBBMonsterSpawnInitializationProcessor::StaticClass(), Created);
                CreatedCount += Created.Num();
                RejectedCount += FMath::Max(0, Batch.Value.Transforms.Num() - Created.Num());
            }
            Current = Manager.GetFragmentDataPtr<FBBBMonsterSpawnFragment>(Entity);
            if (Current && Current->OwnerId == OwnerId && Current->RequestId == RequestId)
            {
                Current->Created += CreatedCount;
                Current->Accepted = FMath::Max(Current->Created, Current->Accepted - RejectedCount);
                if (RejectedCount > 0)
                {
                    Current->Rejections.FindOrAdd(TEXT("创建前安全复核失效")) += RejectedCount;
                }
                Current->bCreating = false;
            }
        });
    }
    if (!SpawnOrder.IsEmpty())
    {
        NextSpawnIndex = (FirstJob + 1) % SpawnOrder.Num();
    }
}

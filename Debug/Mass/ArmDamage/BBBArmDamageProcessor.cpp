#include "BBBArmDamageProcessor.h"

namespace
{
    constexpr double BucketSize = 2.0;
    constexpr int32 MaximumCells = 100000;

    FIntVector Bucket(const FVector& Position)
    {
        return FIntVector(FMath::FloorToInt(Position.X / BucketSize),
            FMath::FloorToInt(Position.Y / BucketSize), FMath::FloorToInt(Position.Z / BucketSize));
    }

    void Flood(const FBBBArmDamageDefinition& Definition, const TBitArray<>& Removed,
        const TArray<int32>& Roots, TBitArray<>& Connected)
    {
        Connected.Init(false, Definition.Cells.Num());
        TArray<int32> Queue;
        Queue.Reserve(Definition.Cells.Num());
        for (const int32 Root : Roots)
        {
            if (!Removed[Root] && !Connected[Root])
            {
                Connected[Root] = true;
                Queue.Add(Root);
            }
        }
        for (int32 Head = 0; Head < Queue.Num(); ++Head)
        {
            for (const int32 Neighbor : Definition.Cells[Queue[Head]].Neighbors)
            {
                if (!Removed[Neighbor] && !Connected[Neighbor])
                {
                    Connected[Neighbor] = true;
                    Queue.Add(Neighbor);
                }
            }
        }
    }
}

bool FBBBArmDamageProcessor::Initialize(FBBBArmDamageRuntimeData& Runtime, TArray<FBBBArmDamageCell> Cells)
{
    auto& State = Runtime.Damage.ArmDamage;
    State.Definition.Reset();
    Reset(Runtime);
    if (Cells.IsEmpty() || Cells.Num() > MaximumCells)
    {
        return false;
    }

    int32 LastStation = 0;
    FBox Bounds(ForceInit);
    for (int32 Index = 0; Index < Cells.Num(); ++Index)
    {
        const auto& Cell = Cells[Index];
        if (Cell.Position.ContainsNaN() || Cell.Position.GetAbsMax() > 1000 || Cell.Station < 0
            || Cell.Neighbors.Num() > 12)
        {
            return false;
        }
        Bounds += Cell.Position;
        LastStation = FMath::Max(LastStation, Cell.Station);
        TSet<int32> Seen;
        for (const int32 Neighbor : Cell.Neighbors)
        {
            if (!Cells.IsValidIndex(Neighbor) || Neighbor == Index || Seen.Contains(Neighbor)
                || !Cells[Neighbor].Neighbors.Contains(Index))
            {
                return false;
            }
            Seen.Add(Neighbor);
        }
    }
    if (LastStation == 0)
    {
        return false;
    }

    auto Definition = MakeShared<FBBBArmDamageDefinition>();
    Definition->Cells = MoveTemp(Cells);
    Definition->MinimumBucket = Bucket(Bounds.Min);
    Definition->MaximumBucket = Bucket(Bounds.Max);
    for (int32 Index = 0; Index < Definition->Cells.Num(); ++Index)
    {
        const auto& Cell = Definition->Cells[Index];
        Definition->SpatialCells.FindOrAdd(Bucket(Cell.Position)).Add(Index);
        if (Cell.Station == 0)
        {
            Definition->ShoulderRoots.Add(Index);
        }
        if (Cell.Station == LastStation)
        {
            Definition->HandRoots.Add(Index);
        }
    }
    if (Definition->ShoulderRoots.IsEmpty() || Definition->HandRoots.IsEmpty())
    {
        return false;
    }

    State.Definition = Definition;
    Reset(Runtime);
    for (int32 Index = 0; Index < State.ShoulderConnected.Num(); ++Index)
    {
        if (!State.ShoulderConnected[Index])
        {
            State.Definition.Reset();
            Reset(Runtime);
            return false;
        }
    }
    return true;
}

void FBBBArmDamageProcessor::Reset(FBBBArmDamageRuntimeData& Runtime)
{
    auto& State = Runtime.Damage.ArmDamage;
    const int32 Count = State.Definition ? State.Definition->Cells.Num() : 0;
    State.Damage.Init(0, Count);
    State.Removed.Init(false, Count);
    State.ShoulderConnected.Init(false, Count);
    State.HandConnected.Init(false, Count);
    State.RemovedCount = 0;
    State.LastVisitedCells = 0;
    State.LastRemovedCells = 0;
    State.HitSerial = 0;
    State.bHandPresent = false;
    State.bHandDetached = false;
    if (State.Definition)
    {
        UpdateConnectivity(State);
    }
}

bool FBBBArmDamageProcessor::ApplyHit(FBBBArmDamageRuntimeData& Runtime, const FBBBArmDamageHit& Hit)
{
    auto& State = Runtime.Damage.ArmDamage;
    if (!State.Definition || Hit.Position.ContainsNaN() || Hit.Direction.ContainsNaN()
        || Hit.Direction.IsNearlyZero() || !FMath::IsFinite(Hit.Radius) || !FMath::IsFinite(Hit.Depth)
        || !FMath::IsFinite(Hit.Strength) || Hit.Radius <= 0 || Hit.Depth <= 0 || Hit.Strength <= 0
        || Hit.Radius > 100 || Hit.Depth > 100)
    {
        return false;
    }

    const FVector Direction = Hit.Direction.GetSafeNormal();
    const double HalfDepth = Hit.Depth * 0.5;
    const FVector Center = Hit.Position + Direction * HalfDepth;
    const double Extent = FMath::Max(Hit.Radius, HalfDepth);
    const FIntVector Minimum = Bucket(Center - FVector(Extent));
    const FIntVector Maximum = Bucket(Center + FVector(Extent));
    const auto& Definition = *State.Definition;
    const FIntVector Lower(FMath::Max(Minimum.X, Definition.MinimumBucket.X),
        FMath::Max(Minimum.Y, Definition.MinimumBucket.Y), FMath::Max(Minimum.Z, Definition.MinimumBucket.Z));
    const FIntVector Upper(FMath::Min(Maximum.X, Definition.MaximumBucket.X),
        FMath::Min(Maximum.Y, Definition.MaximumBucket.Y), FMath::Min(Maximum.Z, Definition.MaximumBucket.Z));
    State.LastVisitedCells = 0;
    State.LastRemovedCells = 0;
    for (int32 X = Lower.X; X <= Upper.X; ++X)
    {
        for (int32 Y = Lower.Y; Y <= Upper.Y; ++Y)
        {
            for (int32 Z = Lower.Z; Z <= Upper.Z; ++Z)
            {
                const auto* Candidates = Definition.SpatialCells.Find(FIntVector(X, Y, Z));
                if (!Candidates)
                {
                    continue;
                }
                for (const int32 Index : *Candidates)
                {
                    ++State.LastVisitedCells;
                    if (State.Removed[Index])
                    {
                        continue;
                    }
                    const FVector Offset = Definition.Cells[Index].Position - Center;
                    const double Along = FVector::DotProduct(Offset, Direction);
                    const double LateralSquared = FMath::Max(0.0, Offset.SizeSquared() - Along * Along);
                    const double DistanceSquared = LateralSquared / FMath::Square(Hit.Radius)
                        + FMath::Square(Along / HalfDepth);
                    if (DistanceSquared >= 1)
                    {
                        continue;
                    }
                    State.Damage[Index] = FMath::Min(1.0f, State.Damage[Index]
                        + Hit.Strength * static_cast<float>(1 - FMath::Sqrt(DistanceSquared)));
                    if (State.Damage[Index] >= 1)
                    {
                        State.Removed[Index] = true;
                        ++State.LastRemovedCells;
                    }
                }
            }
        }
    }
    State.RemovedCount += State.LastRemovedCells;
    ++State.HitSerial;
    if (State.LastRemovedCells > 0)
    {
        UpdateConnectivity(State);
    }
    return true;
}

void FBBBArmDamageProcessor::UpdateConnectivity(FBBBArmDamageState& State)
{
    Flood(*State.Definition, State.Removed, State.Definition->ShoulderRoots, State.ShoulderConnected);
    Flood(*State.Definition, State.Removed, State.Definition->HandRoots, State.HandConnected);
    State.bHandPresent = false;
    State.bHandDetached = true;
    for (const int32 Hand : State.Definition->HandRoots)
    {
        State.bHandPresent |= !State.Removed[Hand];
        if (State.ShoulderConnected[Hand])
        {
            State.bHandDetached = false;
        }
    }
    State.bHandDetached &= State.bHandPresent;
}

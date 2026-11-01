#include "BBBMonsterNavigation.h"

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Movement/BBBMonsterNavigationFragment.h"
#include "NavMesh/RecastQueryFilter.h"
#include "NavMesh/RecastHelpers.h"
#include "DetourCrowd/DetourPathCorridor.h"

void FBBBMonsterNavigation::Reset(FBBBMonsterNavigationFragment& Path)
{
    Path.NativePath.Reset();
    Path.Corridor.Reset();
    Path.Query.Reset();
    Path.Filter.Reset();
    Path.Corners.Reset();
    Path.Walls.Reset();
    Path.bHasPath = false;
    Path.TraversalPoly = 0;
    Path.TraversalLink = FNavLinkId::Invalid;
    Path.RemainingDistance = 0;
    Path.NextBoundaryTime = 0;
}

void FBBBMonsterNavigation::Feedback(FBBBMonsterNavigationFragment& Path, const FVector& Feet)
{
    if (!Path.bHasPath || !Path.Corridor || !Path.Query || !Path.Filter)
    {
        return;
    }
    const auto* Filter = static_cast<const FRecastQueryFilter*>(Path.Filter->GetImplementation());
    const auto Point = Unreal2RecastPoint(Feet);
    Path.Corridor->movePosition(&Point.X, Path.Query.Get(), Filter);
}

FVector FBBBMonsterNavigation::Constrain(const FBBBMonsterNavigationFragment& Path, const FVector& Feet)
{
    if (!Path.bHasPath || !Path.Corridor || !Path.Query || !Path.Filter)
    {
        return Feet;
    }
    const auto* Filter = static_cast<const FRecastQueryFilter*>(Path.Filter->GetImplementation());
    const auto Target = Unreal2RecastPoint(Feet);
    dtReal Result[3];
    dtPolyRef Visited[16];
    int32 Count = 0;
    const dtStatus Status = Path.Query->moveAlongSurface(Path.Corridor->getFirstPoly(),
        Path.Corridor->getPos(), &Target.X, Filter, Result, Visited, &Count, UE_ARRAY_COUNT(Visited));
    if (dtStatusFailed(Status))
    {
        return Recast2UnrealPoint(Path.Corridor->getPos());
    }
    FVector Constrained = Recast2UnrealPoint(Result);
    Constrained.Z = Feet.Z;
    return Constrained;
}

void FBBBMonsterNavigation::FinishTraversal(FBBBMonsterNavigationFragment& Path, const FVector& Feet)
{
    if (Path.TraversalPoly == 0)
    {
        Reset(Path);
        return;
    }
    if (Path.Corridor && Path.Query)
    {
        dtPolyRef Refs[2];
        dtReal Start[3];
        dtReal End[3];
        if (!Path.Corridor->moveOverOffmeshConnection(Path.TraversalPoly, Refs, Path.Corridor->getPos(), Start, End, Path.Query.Get()))
        {
            Reset(Path);
            return;
        }
    }
    Path.TraversalPoly = 0;
    Path.TraversalLink = FNavLinkId::Invalid;
    Path.NextBoundaryTime = 0;
    Feedback(Path, Feet);
}

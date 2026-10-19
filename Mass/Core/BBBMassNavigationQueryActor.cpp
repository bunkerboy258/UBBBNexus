#include "BBBWork/UBBBNexus/Mass/Core/BBBMassNavigationQueryActor.h"

ABBBMassNavigationQueryActor::ABBBMassNavigationQueryActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}

void ABBBMassNavigationQueryActor::SetParameters(const float InStrength, const int32 InPoints,
    const EAPSArcMidpointMode InMode, const float InOffset)
{
    check(IsInGameThread());
    Strength = InStrength;
    Points = InPoints;
    Mode = InMode;
    Offset = InOffset;
}

void ABBBMassNavigationQueryActor::ResetParameters()
{
    SetParameters(0.0f, 0, EAPSArcMidpointMode::SmoothCurve, 0.0f);
}

void ABBBMassNavigationQueryActor::GetMoveToParameters_Implementation(float& ArcStrength, int32& NumMidPoints,
    EAPSArcMidpointMode& MidpointMode, float& LaneOffset) const
{
    ArcStrength = Strength;
    NumMidPoints = Points;
    MidpointMode = Mode;
    LaneOffset = Offset;
}

void ABBBMassNavigationQueryActor::FindCustomPath_Implementation(const FVector& StartLocation, const FVector& EndLocation,
    TArray<FVector>& OutPath, float ArcStrength, UWorld* World, bool& bUseCustomPath, bool& bFound) const
{
    OutPath.Reset();
    bUseCustomPath = false;
    bFound = false;
}

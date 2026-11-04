#pragma once
#include "CoreMinimal.h"
#include "Mass/EntityHandle.h"

/** 单颗弹丸的冻结发射记录与有界命中列表 */
struct FBBBAimDebugShot
{
    FMassEntityHandle Entity;
    uint64 Sequence = 0;
    FGuid AttackId;
    double Expires = 0;
    FTransform Muzzle;
    FVector Direction;
    TArray<TPair<FVector, FVector>> Hits;
};

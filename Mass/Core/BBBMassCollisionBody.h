#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "Mass/EntityHandle.h"

/** 本帧逻辑碰撞场景中的球体快照 */
struct FBBBMassCollisionBody final
{
    /** 目标实体句柄 包含本地代次 */
    FMassEntityHandle Entity;

    /** 世界空间中心 */
    FVector Center = FVector::ZeroVector;

    /** 球体半径 */
    float Radius = 0.0f;
};

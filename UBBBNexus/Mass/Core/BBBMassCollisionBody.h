#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
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

    /** 由碰撞体所属实例发布的物理表面类型 */
    EPhysicalSurface Surface = SurfaceType_Default;

    /** 碰撞体所属实例解释的部位编号 Core 不解释部位语义 */
    uint8 Part = 0;

    /** 复合目标的保守粗筛球 命中候选必须查询所属实例的精细部位 */
    bool bCompound = false;
};

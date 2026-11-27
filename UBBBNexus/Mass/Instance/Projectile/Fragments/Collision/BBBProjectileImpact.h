#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"

/** 仅在本次碰撞批处理中使用的表面命中事实 不保存子弹身份或历史 */
struct FBBBProjectileImpact final
{
    /** 世界空间表面接触点 不是扫掠球心 */
    FVector Position = FVector::ZeroVector;

    /** 世界空间表面外法线 */
    FVector Normal = FVector::UpVector;

    /** UE 物理表面类型 表现资产决定具体反馈 */
    EPhysicalSurface Surface = SurfaceType_Default;

    /** 是否命中静态场景表面（仅静态表面生成弹孔） */
    bool bStaticSurface = false;

    /** 是否为破片手雷破片命中（生成横向撕裂状弹孔） */
    bool bFragment = false;

    /** 世界空间入射方向（用于破片撕裂贴花定向） */
    FVector Direction = FVector::ZeroVector;
};

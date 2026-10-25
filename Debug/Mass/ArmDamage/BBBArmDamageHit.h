#pragma once

#include "CoreMinimal.h"

/** 已转换至左臂参考空间的一次独立测试命中 */
struct FBBBArmDamageHit final
{
    /** UE 网格参考空间中的接触点 厘米 */
    FVector Position = FVector::ZeroVector;

    /** 由创口表面指向组织内部的方向 */
    FVector Direction = FVector::ForwardVector;

    /** 横向毁伤范围 厘米 */
    double Radius = 2.3;

    /** 沿射击方向深入的距离 厘米 */
    double Depth = 4.6;

    /** 本次对邻近组织贡献的损伤量 */
    float Strength = 0.6f;
};

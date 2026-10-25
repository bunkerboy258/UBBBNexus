#pragma once

#include "CoreMinimal.h"

/** 左臂简化结构中一个采样点的制作数据 */
struct FBBBArmDamageCell final
{
    /** UE 网格参考空间中的厘米坐标 */
    FVector Position = FVector::ZeroVector;

    /** 与该采样点直接连通的采样点编号 */
    TArray<int32> Neighbors;

    /** 从肩到手的制作截面编号 */
    int32 Station = 0;
};

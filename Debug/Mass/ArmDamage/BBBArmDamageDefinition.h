#pragma once

#include "BBBArmDamageCell.h"

/** 独立左臂共享的只读连接结构及局部查询索引 */
struct FBBBArmDamageDefinition final
{
    /** 已验证的参考空间采样点与连接 */
    TArray<FBBBArmDamageCell> Cells;

    /** 两厘米空间格中的采样点编号 */
    TMap<FIntVector, TArray<int32>> SpatialCells;

    /** 肩部固定连接的采样点编号 */
    TArray<int32> ShoulderRoots;

    /** 手部最远截面的采样点编号 */
    TArray<int32> HandRoots;

    /** 空间索引查询的最小格 */
    FIntVector MinimumBucket = FIntVector::ZeroValue;

    /** 空间索引查询的最大格 */
    FIntVector MaximumBucket = FIntVector::ZeroValue;
};

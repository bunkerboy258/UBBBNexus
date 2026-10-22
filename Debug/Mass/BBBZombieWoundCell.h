#pragma once

#include "CoreMinimal.h"
#include "BBBZombieWoundCell.generated.h"

/** 编辑阶段预制的骨骼局部表面查询单元 不保存运行命中 */
USTRUCT()
struct FBBBZombieWoundCell final
{
    GENERATED_BODY()

    /** 此单元的参考骨骼 */
    UPROPERTY()
    FName Bone;

    /** 四厘米格子的整数坐标 */
    UPROPERTY()
    FIntVector Coordinate = FIntVector::ZeroValue;

    /** LOD0 索引缓冲的三角形起始位置 */
    UPROPERTY()
    TArray<int32> Triangles;

    /** 同序三角面中心 用于命中时优先选择接触点附近的候选 */
    UPROPERTY()
    TArray<FVector> Centers;
};

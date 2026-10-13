#pragma once
#include "CoreMinimal.h"
#include "BBBMonsterSpawnRegionShape.generated.h"

/** 生成区域的几何形状 */
UENUM(BlueprintType)
enum class EBBBMonsterSpawnRegionShape : uint8
{
    /** 使用三个半尺寸定义方盒 */
    Box UMETA(DisplayName = "方盒"),
    /** X 为半径 Z 为半高 */
    Cylinder UMETA(DisplayName = "圆柱")
};

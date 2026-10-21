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
    Cylinder UMETA(DisplayName = "圆柱"),
    /** 平面圆盘 使用 Z 半高容纳地面投影误差 */
    Disk UMETA(DisplayName = "圆盘")
};

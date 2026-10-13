#pragma once
#include "BBBMonsterSpawnRegionShape.h"
#include "BBBMonsterSpawnRegionDefinition.generated.h"

/** 从地图配置采集的区域纯值快照 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterSpawnRegionDefinition final
{
    GENERATED_BODY()

    /** 在当前副本内唯一的区域名称 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "区域", meta = (DisplayName = "区域标识"))
    FName Id = TEXT("Main");

    /** 地图中的区域坐标与尺寸缩放 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "区域", meta = (DisplayName = "区域变换"))
    FTransform Transform = FTransform::Identity;

    /** 方盒半尺寸 圆柱使用 X 半径与 Z 半高 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "区域", meta = (DisplayName = "区域半尺寸"))
    FVector Extent = FVector(1000.0, 1000.0, 300.0);

    /** 区域的几何构造 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "区域", meta = (DisplayName = "区域形状"))
    EBBBMonsterSpawnRegionShape Shape = EBBBMonsterSpawnRegionShape::Box;

    /** @return 标识与区域尺寸是否有效 */
    bool IsValid() const
    {
        return static_cast<uint8>(Shape) <= static_cast<uint8>(EBBBMonsterSpawnRegionShape::Cylinder)
            && !Id.IsNone() && !Transform.ContainsNaN() && !Extent.ContainsNaN()
            && Extent.GetMin() > 0.0 && Transform.GetScale3D().GetMin() > 0.0;
    }

    /**
     * @param Location	待检查的世界位置
     * @return 世界位置是否位于区域内
     */
    bool Contains(const FVector& Location) const
    {
        const FVector Local = Transform.InverseTransformPosition(Location);
        if (Shape == EBBBMonsterSpawnRegionShape::Cylinder)
        {
            return Local.SizeSquared2D() <= FMath::Square(Extent.X) && FMath::Abs(Local.Z) <= Extent.Z;
        }
        return FMath::Abs(Local.X) <= Extent.X && FMath::Abs(Local.Y) <= Extent.Y && FMath::Abs(Local.Z) <= Extent.Z;
    }
};

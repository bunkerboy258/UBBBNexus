#pragma once
#include "BBBMonsterSpawnRegionShape.h"
#include "BBBMonsterSpawnRegionRole.h"
#include "BBBMonsterSpawnRegionDefinition.generated.h"

class UBBBMonsterSpawnDefinition;

/** 从地图配置采集的区域纯值快照 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterSpawnRegionDefinition final
{
    GENERATED_BODY()

    /** 在当前副本内唯一的区域名称 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "区域", meta = (DisplayName = "区域标识"))
    FName Id = TEXT("Main");

    /** 区域的业务用途 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "区域", meta = (DisplayName = "区域用途"))
    EBBBMonsterSpawnRegionRole Role = EBBBMonsterSpawnRegionRole::Population;

    /** 地图中的区域坐标与尺寸缩放 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "区域", meta = (DisplayName = "区域变换"))
    FTransform Transform = FTransform::Identity;

    /** 方盒半尺寸 圆柱使用 X 半径与 Z 半高 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "区域", meta = (DisplayName = "区域半尺寸"))
    FVector Extent = FVector(1000.0, 1000.0, 300.0);

    /** 区域的几何构造 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "区域", meta = (DisplayName = "区域形状"))
    EBBBMonsterSpawnRegionShape Shape = EBBBMonsterSpawnRegionShape::Box;

    /** 固有刷怪区一次性铺怪数量 仅 Population 使用 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "固有刷怪", meta = (DisplayName = "固有数量", ClampMin = "1", ClampMax = "256"))
    int32 PopulationCount = 12;

    /** 固有刷怪区激活半径覆盖 零表示使用导演默认半径 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "固有刷怪", meta = (DisplayName = "激活半径覆盖", ClampMin = "0", Units = "cm"))
    float ActivationRadius = 0.0f;

    /** 固有刷怪区可选覆盖的出生配置 为空使用导演默认配置 */
    TWeakObjectPtr<UBBBMonsterSpawnDefinition> SpawnDefinitionOverride;

    /** @return 标识与区域尺寸是否有效 */
    bool IsValid() const
    {
        return static_cast<uint8>(Shape) <= static_cast<uint8>(EBBBMonsterSpawnRegionShape::Disk)
            && static_cast<uint8>(Role) <= static_cast<uint8>(EBBBMonsterSpawnRegionRole::BlockSpawn)
            && !Id.IsNone() && !Transform.ContainsNaN() && !Extent.ContainsNaN()
            && Extent.GetMin() > 0.0 && Transform.GetScale3D().GetMin() > 0.0
            && PopulationCount >= 1 && FMath::IsFinite(ActivationRadius) && ActivationRadius >= 0.0f;
    }

    /**
     * @param Location	待检查的世界位置
     * @return 世界位置是否位于区域内
     */
    bool Contains(const FVector& Location) const
    {
        const FVector Local = Transform.InverseTransformPosition(Location);
        if (Shape != EBBBMonsterSpawnRegionShape::Box)
        {
            return Local.SizeSquared2D() <= FMath::Square(Extent.X) && FMath::Abs(Local.Z) <= Extent.Z;
        }
        return FMath::Abs(Local.X) <= Extent.X && FMath::Abs(Local.Y) <= Extent.Y && FMath::Abs(Local.Z) <= Extent.Z;
    }

    /**
     * @param Location	待检查的世界位置
     * @return 点到区域包围体的近似世界距离 区域内返回零
     */
    double DistanceToPoint(const FVector& Location) const
    {
        const FVector Local = Transform.InverseTransformPosition(Location);
        const FVector Scale = Transform.GetScale3D().GetAbs();
        if (Shape != EBBBMonsterSpawnRegionShape::Box)
        {
            const double Radial = FMath::Max(0.0, Local.Size2D() - Extent.X) * FMath::Max(Scale.X, Scale.Y);
            const double Vertical = FMath::Max(0.0, FMath::Abs(Local.Z) - Extent.Z) * Scale.Z;
            return FMath::Sqrt(Radial * Radial + Vertical * Vertical);
        }
        const double Dx = FMath::Max(0.0, FMath::Abs(Local.X) - Extent.X) * Scale.X;
        const double Dy = FMath::Max(0.0, FMath::Abs(Local.Y) - Extent.Y) * Scale.Y;
        const double Dz = FMath::Max(0.0, FMath::Abs(Local.Z) - Extent.Z) * Scale.Z;
        return FMath::Sqrt(Dx * Dx + Dy * Dy + Dz * Dz);
    }
};

#pragma once
#include "Components/BoxComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnRegionDefinition.h"
#include "BBBMonsterSpawnRegionPresentationComponent.generated.h"

class UBBBMonsterSpawnDefinition;

/** 地图中的生成区域配置与编辑器桥接 */
UCLASS(ClassGroup = (BBB), meta = (BlueprintSpawnableComponent, DisplayName = "小怪生成区域"))
class ABBB_EVAC_API UBBBMonsterSpawnRegionPresentationComponent final : public UBoxComponent
{
    GENERATED_BODY()
public:
    /** @return 构造仅用于配置显示的区域组件 */
    UBBBMonsterSpawnRegionPresentationComponent();

    /** 在所属副本中唯一的区域标识 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "生成区域", meta = (DisplayName = "区域标识"))
    FName RegionId = TEXT("Main");

    /** 区域的业务用途 区分固有刷怪区 预制波次区与禁刷区 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "生成区域", meta = (DisplayName = "区域用途"))
    EBBBMonsterSpawnRegionRole Role = EBBBMonsterSpawnRegionRole::Population;

    /** 圆柱使用方盒 X 半尺寸作为半径 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "生成区域", meta = (DisplayName = "区域形状"))
    EBBBMonsterSpawnRegionShape Shape = EBBBMonsterSpawnRegionShape::Box;

    /** 固有刷怪区一次性铺怪数量 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "固有刷怪", meta = (DisplayName = "固有数量", ClampMin = "1", ClampMax = "256", EditCondition = "Role == EBBBMonsterSpawnRegionRole::Population"))
    int32 PopulationCount = 12;

    /** 固有刷怪区激活半径覆盖 零表示使用导演默认半径 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "固有刷怪", meta = (DisplayName = "激活半径覆盖", ClampMin = "0", Units = "cm", EditCondition = "Role == EBBBMonsterSpawnRegionRole::Population"))
    float ActivationRadius = 0.0f;

    /** 固有刷怪区可选覆盖的出生配置 为空使用导演默认配置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "固有刷怪", meta = (DisplayName = "出生配置覆盖", EditCondition = "Role == EBBBMonsterSpawnRegionRole::Population"))
    TObjectPtr<UBBBMonsterSpawnDefinition> SpawnDefinitionOverride;

    /** @return 当前区域的不可写玩法配置副本 */
    FBBBMonsterSpawnRegionDefinition ReadDefinition() const;
};

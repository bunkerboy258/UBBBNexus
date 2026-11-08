#pragma once
#include "Components/BoxComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnRegionDefinition.h"
#include "BBBMonsterSpawnRegionPresentationComponent.generated.h"

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

    /** 圆柱使用方盒 X 半尺寸作为半径 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "生成区域", meta = (DisplayName = "区域形状"))
    EBBBMonsterSpawnRegionShape Shape = EBBBMonsterSpawnRegionShape::Box;

    /** @return 当前区域的不可写玩法配置副本 */
    FBBBMonsterSpawnRegionDefinition ReadDefinition() const;
};

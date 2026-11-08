#pragma once
#include "GameFramework/Actor.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnRegionDefinition.h"
#include "BBBMonsterSpawnRegionPresentationActor.generated.h"

class UBBBMonsterSpawnRegionPresentationComponent;

/** 无独立玩法状态的地图生成区域入口 */
UCLASS(Blueprintable, meta = (DisplayName = "小怪生成区域"))
class ABBB_EVAC_API ABBBMonsterSpawnRegionPresentationActor : public AActor
{
    GENERATED_BODY()
public:
    /** @return 创建区域组件 */
    ABBBMonsterSpawnRegionPresentationActor();
    /** @return 当前区域配置 */
    FBBBMonsterSpawnRegionDefinition ReadDefinition() const;
    /** @return 编辑器区域组件 */
    UBBBMonsterSpawnRegionPresentationComponent* GetRegionComponent() const;
private:
    /** 区域尺寸与形状配置 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "生成区域", meta = (AllowPrivateAccess = "true", DisplayName = "生成区域"))
    TObjectPtr<UBBBMonsterSpawnRegionPresentationComponent> Region;
};

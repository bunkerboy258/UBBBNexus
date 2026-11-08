#pragma once
#include "CoreMinimal.h"
#include "BBBMonsterSpawnPoolEntry.generated.h"

class UBBBMonsterDefinition;

/** 小怪组合中的一个带权重模板 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterSpawnPoolEntry final
{
    GENERATED_BODY()

    /** 直接引用当前小怪静态定义 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "组合", meta = (DisplayName = "小怪定义"))
    TObjectPtr<UBBBMonsterDefinition> Definition;

    /** 为零时不参与选择 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "组合", meta = (DisplayName = "选择权重", ClampMin = "0"))
    float Weight = 1.0f;
};

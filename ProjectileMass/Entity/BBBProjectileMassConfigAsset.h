#pragma once

#include "MassEntityConfigAsset.h"
#include "BBBProjectileMassConfigAsset.generated.h"

/** 提供弹丸实体特征和实例化网格的共享配置 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBProjectileMassConfigAsset final : public UMassEntityConfigAsset
{
    GENERATED_BODY()

public:
    /**
     * 创建包含ProjectileMass特征的实体配置
     * @return 无
     */
    UBBBProjectileMassConfigAsset();
};

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "BBBMonsterHealthFragment.generated.h"

/** 小怪生命值状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterHealthFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 根据血量上限与伤害字典计算的本机缓存 不接受独立配置或网络写入 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "小怪", meta = (DisplayName = "剩余血量"))
    float CurrentHealth = 100.0f;

    /** 整体传导生命缓存 致死部位损毁时仍可大于零 */
    float MainHealth = 100.0f;

    /** 六部位损毁位图 只由当前累计贡献推导 */
    uint8 DestroyedParts = 0;

    /** 上次已解析的累计部位伤害总量 用于本轮变化检测 */
    double ResolvedDamage = 0.0;

    /** 同一代小怪各端一致的血量上限 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "小怪", meta = (DisplayName = "血量上限"))
    float MaxHealth = 100.0f;

    /** 死亡表现保留到实体回收的时长 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (ClampMin = "0.01", DisplayName = "死亡后存活时间"))
    float CorpseLifetime = 20.0f;
};

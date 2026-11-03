#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterPickupConfig.generated.h"
/** 角色自动拾取的静态配置 */
USTRUCT(BlueprintType)
struct FBBBCharacterPickupConfig final
{
    GENERATED_BODY()
    /** 自动探测与领取距离 单位厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "拾取", meta = (DisplayName = "拾取距离", ClampMin = "1", ToolTip = "角色与世界掉落物之间允许发起自动领取的最大距离 单位厘米"))
    float Distance = 150.0f;
    /** 无交付时的探测间隔 单位秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "拾取", meta = (DisplayName = "探测间隔", ClampMin = "0.05", ToolTip = "附近世界掉落物的探测间隔 单位秒"))
    float ProbeInterval = 0.2f;
    /** 入包拒绝后的重试间隔 单位秒 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "拾取", meta = (DisplayName = "拒绝重试间隔", ClampMin = "0.1", ToolTip = "满包等拒绝结果后再次尝试领取的间隔 单位秒"))
    float RejectedRetryInterval = 1.0f;
};

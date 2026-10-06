#pragma once

#include "Engine/DataAsset.h"
#include "BBBMonsterBloodPresentationDefinition.generated.h"

class UNiagaraDataChannelAsset;
class UMaterialInterface;

/** 僵尸血粒子与地面血迹静态配置 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMonsterBloodPresentationDefinition final : public UDataAsset
{
    GENERATED_BODY()

public:
    /** 空间岛批量血粒子通道 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "血粒子通道"))
    TObjectPtr<UNiagaraDataChannelAsset> ImpactChannel;

    /** 地面随机血迹材质 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "地面血迹材质"))
    TArray<TObjectPtr<UMaterialInterface>> GroundMaterials;

    /** 一个世界最多保留的地面血迹数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "地面血迹上限", ClampMin = "1", ClampMax = "256"))
    int32 MaximumDecals = 96;

    /** 地面血迹完整显示后淡出 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "血迹保留秒数", ClampMin = "1.0", Units = "s"))
    float DecalLifetime = 24.0f;

    /** 连射同一区域血迹的最小间距 厘米 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "血迹最小间距", ClampMin = "1.0", Units = "cm"))
    float DecalSpacing = 24.0f;
};

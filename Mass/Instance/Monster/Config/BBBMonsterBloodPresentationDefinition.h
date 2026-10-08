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

    /** 方向性主痕迹 不使用积液轮廓代替飞溅 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "主飞溅材质"))
    TArray<TObjectPtr<UMaterialInterface>> SplatterMaterials;

    /** 外围零散血滴遮罩 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "细滴材质"))
    TArray<TObjectPtr<UMaterialInterface>> DropletMaterials;

    /** 密集落点使用的不规则积血遮罩 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "积血材质"))
    TArray<TObjectPtr<UMaterialInterface>> PoolMaterials;

    /** 一个世界最多保留的地面血迹数量 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "地面血迹上限", ClampMin = "1", ClampMax = "256"))
    int32 MaximumDecals = 192;

    /** 地面血迹完整显示后淡出 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "血迹保留秒数", ClampMin = "1.0", Units = "s"))
    float DecalLifetime = 120.0f;

    /** 细滴的完整保留时间 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "细滴保留秒数", ClampMin = "1.0", Units = "s"))
    float DropletLifetime = 50.0f;

    /** 同一表面局部范围内的主痕迹上限 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "局部主痕迹上限", ClampMin = "2", ClampMax = "12"))
    int32 LocalDecalLimit = 5;

    /** 局部积累检查半径 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "局部积累半径", ClampMin = "10.0", Units = "cm"))
    float AccumulationRadius = 65.0f;

    /** 世界当前飞行的代表性血滴上限 不是命中记录 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "飞行血滴上限", ClampMin = "3", ClampMax = "128"))
    int32 MaximumFlights = 64;

    /** 每帧环境碰撞与边缘检查合计上限 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "小怪|血效", meta = (DisplayName = "每帧采样上限", ClampMin = "8", ClampMax = "256"))
    int32 MaximumTracesPerFrame = 96;
};

#pragma once

#include "UObject/Object.h"
#include "BBBMonsterBloodSprayPresentation.h"
#include "BBBMonsterBloodPresentationDefinition.generated.h"

class UNiagaraDataChannelAsset;
class UMaterialInterface;

/** 僵尸血粒子与地面血迹静态配置 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced, HideDropdown)
class ABBB_EVAC_API UBBBMonsterBloodPresentationDefinition final : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category="血效|空中喷血", meta=(DisplayName="普通命中喷血"))
    FBBBMonsterBloodSprayPresentation HitSpray;

    UPROPERTY(EditAnywhere, Category="血效|空中喷血", meta=(DisplayName="断肢喷血"))
    FBBBMonsterBloodSprayPresentation SeveringSpray;

    /** 关闭后不写入空中血粒子通道 不影响环境血迹 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|开关", meta = (DisplayName = "启用空中喷血"))
    bool bEnableParticles = true;

    /** 关闭后不生成落地采样血滴 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|开关", meta = (DisplayName = "启用环境血迹"))
    bool bEnableResidue = true;

    /** 向外 沿入射切线 向上的喷射方向权重 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|喷射", meta = (DisplayName = "喷射方向权重", ClampMin = "0"))
    FVector SprayDirectionWeights = FVector(0.75, 0.25, 0.25);

    /** 普通命中用于落地碰撞的代表性血滴数量范围 不是空中粒子数量 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|落地血滴", meta = (DisplayName = "普通命中血滴数量范围"))
    FIntPoint DropletCount = FIntPoint(2, 3);

    /** 断开时包含延迟滴落在内的代表性血滴数量 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|落地血滴", meta = (DisplayName = "断肢血滴数量", ClampMin = "0", ClampMax = "32"))
    int32 SeveringDropletCount = 6;

    /** 沿喷射方向的初速度最小值与最大值 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|落地血滴", meta = (DisplayName = "喷射速度范围", Units = "cm/s"))
    FVector2D DropletSpeed = FVector2D(520, 800);

    /** 横向散开的速度范围 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|落地血滴", meta = (DisplayName = "散射速度范围", Units = "cm/s"))
    FVector2D DropletSpreadSpeed = FVector2D(80, 220);

    /** 沿命中法线离开身体的距离 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|落地血滴", meta = (DisplayName = "离体偏移", ClampMin = "0", Units = "cm"))
    float SurfaceOffset = 6;

    /** 断开后的短促滴落向下初速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|断口滴血", meta = (DisplayName = "断口滴落速度", ClampMin = "0", Units = "cm/s"))
    float SeveringDripSpeed = 80;

    /** 延迟滴落的横向速度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|断口滴血", meta = (DisplayName = "断口滴落散射速度", ClampMin = "0", Units = "cm/s"))
    float SeveringDripSpread = 45;

    /** 相邻延迟滴落的间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|断口滴血", meta = (DisplayName = "断口滴落间隔", ClampMin = "0", ClampMax = "0.5", Units = "s"))
    float SeveringDripInterval = 0.12f;

    /** 主飞溅贴花的半长范围 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|环境残留", meta = (DisplayName = "主飞溅半长范围", Units = "cm"))
    FVector2D SplatterHalfLength = FVector2D(30, 48);

    /** 细滴贴花的半长范围 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|环境残留", meta = (DisplayName = "细滴半长范围", Units = "cm"))
    FVector2D FineHalfLength = FVector2D(10, 24);

    /** 主痕迹相对于材质宽高比的宽度范围 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|环境残留", meta = (DisplayName = "主飞溅宽度比例范围"))
    FVector2D SplatterWidthRatio = FVector2D(0.4, 0.65);

    /** 细滴和积血的宽度比例范围 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|环境残留", meta = (DisplayName = "细滴积血宽度比例范围"))
    FVector2D OtherWidthRatio = FVector2D(0.6, 0.9);

    /** 相对血滴飞行切线的随机旋转幅度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|环境残留", meta = (DisplayName = "方向随机角度", ClampMin = "0", ClampMax = "180", Units = "deg"))
    float DirectionJitter = 18;

    /** 同一积血再次接收落点时增加的覆盖量 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|环境残留", meta = (DisplayName = "积血增长量", ClampMin = "0", ClampMax = "1"))
    float PoolCoverageStep = 0.12f;

    /** 完整保留结束后的淡出时间 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "血效|环境残留", meta = (DisplayName = "血迹淡出秒数", ClampMin = "0.1", Units = "s"))
    float FadeDuration = 4;

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

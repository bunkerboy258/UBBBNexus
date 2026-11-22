#pragma once
#include "Engine/DataAsset.h"
#include "BBBMonsterDirectorEventWaveDefinition.h"
#include "BBBMonsterDirectorDefinition.generated.h"

class UBBBMonsterSpawnDefinition;

/** 推进式关卡导演的共享静态规则 包含固有僵尸 动态波次与预制波次 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMonsterDirectorDefinition final : public UDataAsset
{
    GENERATED_BODY()
public:
    /** 同一世界内的存活小怪与预留数量上限 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "全局", meta = (DisplayName = "世界存活上限", ClampMin = "1"))
    int32 WorldMaxAlive = 80;

    /** 每名额外玩家增加的波次数量比例 世界上限不随人数增加 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "全局", meta = (DisplayName = "额外玩家数量倍率", ClampMin = "0", ClampMax = "1"))
    float AdditionalPlayerScale = 0.0f;

    /** 活怪超过此距离且不在任意玩家视野内时静默回收 零表示不回收 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "全局", meta = (DisplayName = "远距离回收半径", ClampMin = "0", Units = "cm"))
    float DespawnDistance = 8500.0f;

    /** 无可用位置或额度时的重试间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "全局", meta = (DisplayName = "重试间隔", ClampMin = "0.1", Units = "s"))
    float RetryInterval = 2.0f;

    /** 超时查询失效并释放预留额度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "全局", meta = (DisplayName = "选点超时", ClampMin = "1", Units = "s"))
    float QueryTimeout = 10.0f;

    /** 导演规则的最短采样间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "全局", meta = (DisplayName = "导演更新间隔", ClampMin = "0.05", Units = "s"))
    float UpdateInterval = 0.25f;

    /** 固有刷怪区的默认出生配置 区域未覆写时使用 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "固有僵尸", meta = (DisplayName = "默认固有出生配置"))
    TObjectPtr<UBBBMonsterSpawnDefinition> PopulationSpawnDefinition;

    /** 玩家靠近固有区域多少距离时触发一次性铺怪 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "固有僵尸", meta = (DisplayName = "默认激活半径", ClampMin = "500", Units = "cm"))
    float PopulationActivationRadius = 6000.0f;

    /** 是否启用周期性动态波次 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "动态波次", meta = (DisplayName = "启用动态波次"))
    bool bEnableDynamicHorde = true;

    /** 动态波次使用的出生配置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "动态波次", meta = (DisplayName = "动态波次出生配置", EditCondition = "bEnableDynamicHorde"))
    TObjectPtr<UBBBMonsterSpawnDefinition> DynamicHordeSpawnDefinition;

    /** 动态波次最短间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "动态波次", meta = (DisplayName = "最短波次间隔", ClampMin = "5", Units = "s", EditCondition = "bEnableDynamicHorde"))
    float DynamicHordeMinInterval = 60.0f;

    /** 动态波次最长间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "动态波次", meta = (DisplayName = "最长波次间隔", ClampMin = "5", Units = "s", EditCondition = "bEnableDynamicHorde"))
    float DynamicHordeMaxInterval = 90.0f;

    /** 每次动态波次刷怪数量 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "动态波次", meta = (DisplayName = "单波数量", ClampMin = "1", ClampMax = "128", EditCondition = "bEnableDynamicHorde"))
    int32 DynamicHordeBatchCount = 12;

    /** 围绕存活玩家动态采样区域的水平半径 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "动态波次", meta = (DisplayName = "玩家周边采样半径", ClampMin = "500", Units = "cm", EditCondition = "bEnableDynamicHorde"))
    float DynamicHordeSampleRadius = 4000.0f;

    /** 围绕存活玩家动态采样区域的垂直半高 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "动态波次", meta = (DisplayName = "玩家周边采样半高", ClampMin = "100", Units = "cm", EditCondition = "bEnableDynamicHorde"))
    float DynamicHordeSampleHalfHeight = 600.0f;

    /** 运行预制事件波次期间自动暂停动态波次计时 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "动态波次", meta = (DisplayName = "事件期间暂停动态波次", EditCondition = "bEnableDynamicHorde"))
    bool bPauseDynamicHordeDuringEvent = true;

    /** 按事件标识配置的预制波次表 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "预制波次", meta = (DisplayName = "预制波次列表"))
    TArray<FBBBMonsterDirectorEventWaveDefinition> EventWaves;

    /**
     * @param EventId	事件标识
     * @return 匹配的预制波次配置指针 未找到返回空
     */
    const FBBBMonsterDirectorEventWaveDefinition* FindEventWave(FName EventId) const;

    /**
     * @param Error	无效配置的中文原因
     * @return 全部规则与参数是否有效
     */
    bool Validate(FString& Error) const;

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};

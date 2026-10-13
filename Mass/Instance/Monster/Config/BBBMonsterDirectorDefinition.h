#pragma once
#include "Engine/DataAsset.h"
#include "BBBMonsterDirectorStageDefinition.h"
#include "BBBMonsterDirectorDefinition.generated.h"

class UEnvQuery;

/** 副本小怪导演的共享静态规则 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMonsterDirectorDefinition final : public UDataAsset
{
    GENERATED_BODY()
public:
    /** 按顺序执行的副本阶段 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "副本导演", meta = (DisplayName = "阶段列表"))
    TArray<FBBBMonsterDirectorStageDefinition> Stages;

    /** 生成区域候选点查询 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "出生检查", meta = (DisplayName = "出生位置查询"))
    TObjectPtr<UEnvQuery> PlacementQuery;

    /** 同一世界内的存活小怪与预留数量上限 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "数量", meta = (DisplayName = "世界存活上限", ClampMin = "1"))
    int32 WorldMaxAlive = 80;

    /** 每次选点的有界候选数量 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "出生检查", meta = (DisplayName = "候选点数量", ClampMin = "8", ClampMax = "512"))
    int32 CandidateCount = 96;

    /** 出生位置距离全部有效玩家的最小距离 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "出生检查", meta = (DisplayName = "玩家安全距离", ClampMin = "0", Units = "cm"))
    float MinPlayerDistance = 1500.0f;

    /** 同批出生点的最小水平间距 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "出生检查", meta = (DisplayName = "小怪出生间距", ClampMin = "1", Units = "cm"))
    float MinSpacing = 120.0f;

    /** 无可用位置或额度时的重试间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "时间", meta = (DisplayName = "重试间隔", ClampMin = "0.1", Units = "s"))
    float RetryInterval = 2.0f;

    /** 超时查询失效并释放预留额度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "时间", meta = (DisplayName = "选点超时", ClampMin = "1", Units = "s"))
    float QueryTimeout = 10.0f;

    /** 导演规则的最短采样间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "时间", meta = (DisplayName = "导演更新间隔", ClampMin = "0.05", Units = "s"))
    float UpdateInterval = 0.25f;

    /** 出生点必须与至少一名有效玩家具有完整导航路径 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "出生检查", meta = (DisplayName = "检查玩家可达路径"))
    bool bRequirePath = true;

    /** 拒绝处于玩家前方视野且无遮挡的出生位置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "出生检查", meta = (DisplayName = "避开玩家可见位置"))
    bool bAvoidVisible = true;

    /**
     * @param Error	无效配置的中文原因
     * @return 全部阶段与参数是否有效
     */
    bool Validate(FString& Error) const;

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};

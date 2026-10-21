#pragma once
#include "Engine/DataAsset.h"
#include "BBBMonsterDirectorStageDefinition.h"
#include "BBBMonsterDirectorDefinition.generated.h"


/** 副本小怪导演的共享静态规则 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMonsterDirectorDefinition final : public UDataAsset
{
    GENERATED_BODY()
public:
    /** 按顺序执行的副本阶段 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "副本导演", meta = (DisplayName = "阶段列表"))
    TArray<FBBBMonsterDirectorStageDefinition> Stages;

    /** 同一世界内的存活小怪与预留数量上限 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "数量", meta = (DisplayName = "世界存活上限", ClampMin = "1"))
    int32 WorldMaxAlive = 80;

    /** 无可用位置或额度时的重试间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "时间", meta = (DisplayName = "重试间隔", ClampMin = "0.1", Units = "s"))
    float RetryInterval = 2.0f;

    /** 超时查询失效并释放预留额度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "时间", meta = (DisplayName = "选点超时", ClampMin = "1", Units = "s"))
    float QueryTimeout = 10.0f;

    /** 导演规则的最短采样间隔 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "时间", meta = (DisplayName = "导演更新间隔", ClampMin = "0.05", Units = "s"))
    float UpdateInterval = 0.25f;

    /**
     * @param Error	无效配置的中文原因
     * @return 全部阶段与参数是否有效
     */
    bool Validate(FString& Error) const;

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};

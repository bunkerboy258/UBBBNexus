#pragma once

#include "Engine/DeveloperSettings.h"
#include "BBBMonsterNavigationSettings.generated.h"

/** 全世界僵尸移动寻路的提交预算 不由单个怪物资产覆盖 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "僵尸寻路预算"))
class ABBB_EVAC_API UBBBMonsterNavigationSettings final : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    /** 一个请求包含站位检查 投影和 APS 查询 */
    UPROPERTY(Config, EditAnywhere, Category = "寻路预算", meta = (DisplayName = "每帧请求上限", ClampMin = "1", ClampMax = "256"))
    int32 MaxRequestsPerFrame = 16;

    /** 达到预算后停止提交 后台没有额外请求队列 单次同步查询无法中断 */
    UPROPERTY(Config, EditAnywhere, Category = "寻路预算", meta = (DisplayName = "每帧提交时间预算", ToolTip = "软预算 单次同步查询可能超出此值 不包含常规实体状态遍历", Units = "ms", ClampMin = "0.1", ClampMax = "16"))
    float TimeBudgetMs = 1.0f;

    /** 每帧可更新局部净空与绕行的实体数 按轮次公平分摊 */
    UPROPERTY(Config, EditAnywhere, Category = "局部跟随预算", meta = (DisplayName = "每帧局部检查上限", ClampMin = "1", ClampMax = "256"))
    int32 MaxLocalChecksPerFrame = 32;

    /** 单次局部检查仍是不可中断查询 达到软预算后从下个实体继续 */
    UPROPERTY(Config, EditAnywhere, Category = "局部跟随预算", meta = (DisplayName = "每帧局部检查时间预算", Units = "ms", ClampMin = "0.1", ClampMax = "8"))
    float LocalTimeBudgetMs = 0.75f;

    /** 基础参考路径仍计入总 APS 调用预算 不逐只逐帧双算 */
    UPROPERTY(Config, EditAnywhere, Category = "路线质量预算", meta = (DisplayName = "每帧参考查询上限", ClampMin = "0", ClampMax = "16"))
    int32 MaxReferenceQueriesPerFrame = 2;

    /** @return 当前项目级预算配置是否有效 */
    bool IsValid() const
    {
        return MaxRequestsPerFrame >= 1 && MaxRequestsPerFrame <= 256
            && MaxLocalChecksPerFrame >= 1 && MaxLocalChecksPerFrame <= 256
            && FMath::IsFinite(LocalTimeBudgetMs) && LocalTimeBudgetMs >= 0.1f && LocalTimeBudgetMs <= 8.0f
            && MaxReferenceQueriesPerFrame >= 0 && MaxReferenceQueriesPerFrame <= 16
            && FMath::IsFinite(TimeBudgetMs) && TimeBudgetMs >= 0.1f && TimeBudgetMs <= 16.0f;
    }

    /** @return 项目设置中的游戏分类 */
    virtual FName GetCategoryName() const override
    {
        return TEXT("Game");
    }
};

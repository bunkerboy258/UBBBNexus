#pragma once
#include "Engine/DataAsset.h"
#include "BBBMonsterSpawnDefinition.generated.h"

class UBBBMonsterSpawnPoolDefinition;

/** 独立生成与导演共用的出生规则 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBMonsterSpawnDefinition final : public UDataAsset
{
    GENERATED_BODY()
public:
    /** 模板权重只表示抽样概率 不保证每批比例 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "出生", meta = (DisplayName = "小怪组合"))
    TObjectPtr<UBBBMonsterSpawnPoolDefinition> Pool;

    /** 一次请求最多检查的候选点 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "出生", meta = (DisplayName = "候选数量", ClampMin = "1", ClampMax = "512"))
    int32 CandidateCount = 96;

    /** 一个世界每帧最多处理的候选点 所有入口共享最严格预算 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "预算", meta = (DisplayName = "每帧候选上限", ClampMin = "1", ClampMax = "64"))
    int32 CandidatesPerFrame = 8;

    /** 一个世界每帧最多实际创建的实体数 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "预算", meta = (DisplayName = "每帧创建上限", ClampMin = "1", ClampMax = "64"))
    int32 CreatesPerFrame = 8;

    /** 单个候选最多检查的玩家数 同时限制路径和射线查询数量 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "预算", meta = (DisplayName = "玩家检查上限", ClampMin = "1", ClampMax = "16"))
    int32 MaxObservers = 8;

    /** 出生点与所有玩家保持的安全距离 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "出生检查", meta = (DisplayName = "玩家安全距离", ClampMin = "0", Units = "cm"))
    float MinPlayerDistance = 1500.0f;

    /** 已有小怪与本批位置都参与间距检查 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "出生检查", meta = (DisplayName = "出生间距", ClampMin = "1", Units = "cm"))
    float MinSpacing = 120.0f;

    /** 导航投影后地面射线的最大半长度 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "出生检查", meta = (DisplayName = "地面检查距离", ClampMin = "1", Units = "cm"))
    float GroundTraceDistance = 100.0f;

    /** 是否要求与玩家有完整导航路径 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "出生检查", meta = (DisplayName = "检查可达路径"))
    bool bRequirePath = true;

    /** 是否拒绝玩家视野内无遮挡位置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "出生检查", meta = (DisplayName = "避开可见位置"))
    bool bAvoidVisible = true;

    /** 随机种子为零时由房主为每个入口选择一次 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "随机", meta = (DisplayName = "随机种子"))
    int32 Seed = 0;

    /** 关闭后使用固定世界偏航角 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "随机", meta = (DisplayName = "随机出生朝向"))
    bool bRandomYaw = true;

    /** 固定出生朝向 不影响运行中的转向 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "随机", meta = (DisplayName = "固定偏航角", EditCondition = "!bRandomYaw", Units = "deg"))
    float FixedYaw = 0.0f;

    /**
     * @param Error	无效配置的具体原因
     * @return 全部出生参数是否有效
     */
    bool Validate(FString& Error) const;
};

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Perception/BBBMonsterVisualContact.h"

#include "BBBMonsterPerceptionFragment.generated.h"

/** 小怪侦察配置 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterPerceptionFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 能够发现玩家的最大距离 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (DisplayName = "视野范围"))
    float SightRange = 3500.0f;

    /** 当前玩家视觉结果 仅保留仍有效的玩家 */
    TArray<FBBBMonsterVisualContact> Contacts;

    /** 下次视觉采样时间 */
    float NextSenseTime = 0.0f;

    /** 上次采样时间 用于按实际间隔累计警觉 */
    float LastSenseTime = -1.0f;

    /** 持续占优的新候选 */
    TWeakObjectPtr<AActor> SwitchCandidate;

    /** 候选开始持续占优的时刻 */
    float SwitchStartedAt = 0.0f;

    /** 自己确认目标后允许再次示警的时刻 */
    float NextAllyAlertTime = 0.0f;

    /** 已消费的最近有效伤害时间 */
    double LastDamageTime = -FLT_MAX;

    /** 到达位置后抑制重复线索的时刻 */
    float InvestigationFinishedAt = -FLT_MAX;
};

/** 当前视觉结果数组使用原生构造与析构 */
template<>
struct TMassFragmentTraits<FBBBMonsterPerceptionFragment>
{
    enum
    {
        AuthorAcceptsItsNotTriviallyCopyable = true
    };
};

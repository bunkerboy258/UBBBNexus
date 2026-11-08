#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"

#include "BBBMonsterBehaviorFragment.generated.h"

/** 小怪行为状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterBehaviorFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 当前权威状态 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (DisplayName = "行为状态"))
    EBBBMonsterBehavior State = EBBBMonsterBehavior::Idle;

    /** 当前状态进入的世界时间 */
    float StateEnteredTime = 0.0f;

    /** 状态切换或同状态动作重启时递增 */
    uint32 ActionId = 0;

    /** 限时行为结束时刻 负值表示尚未初始化 */
    float StateEndsAtTime = -1.0f;

    /** 待机停留的最短时间 */
    float IdleDurationMin = 2.0f;

    /** 待机停留的最长时间 */
    float IdleDurationMax = 4.0f;

    /** 巡逻步行的最短时间 */
    float PatrolDurationMin = 2.0f;

    /** 巡逻步行的最长时间 */
    float PatrolDurationMax = 5.0f;

    /** 发现或丢失目标后的警觉停留时间 */
    float AlertDuration = 1.2f;

    /** 每只实体独立的行为随机流 */
    FRandomStream Random;

    /** 主机上一轮是否持有有效目标 */
    bool bHadTarget = false;
};

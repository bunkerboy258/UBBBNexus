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
};

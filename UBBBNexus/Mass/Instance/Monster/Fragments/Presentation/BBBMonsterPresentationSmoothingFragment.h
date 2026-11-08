#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "BBBMonsterPresentationSmoothingFragment.generated.h"

class AActor;

/** 客机模型追靠最新逻辑结果的显示状态 不参与玩法与碰撞 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterPresentationSmoothingFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 已应用给模型的显示变换 */
    FTransform DisplayTransform = FTransform::Identity;

    /** 上次接收显示变换的演员 用于识别首次出现与演员替换 */
    TWeakObjectPtr<AActor> LastActor;
};

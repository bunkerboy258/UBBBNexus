#pragma once

#include "CoreMinimal.h"
#include "BBBAimConfig.generated.h"

/** 瞄准动画表现配置 */
USTRUCT(BlueprintType)
struct FBBBAimAnimationConfig
{
    GENERATED_BODY()

    /** 计算瞄准IK起点时使用的角色骨骼 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName AimIKOriginBoneName = FName("spine_03");

};

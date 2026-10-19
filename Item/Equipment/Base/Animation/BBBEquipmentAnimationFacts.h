#pragma once

#include "CoreMinimal.h"
#include "BBBEquipmentAnimationFacts.generated.h"

/** 装备动画图读取的只读表现事实 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBEquipmentAnimationFacts
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "右手空间左手目标"))
    FVector LeftHandTargetHandRSpace = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "存在左手目标"))
    bool bHasLeftHandTarget = false;

};

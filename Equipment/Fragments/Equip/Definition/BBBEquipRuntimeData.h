#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBEquipRuntimeData.generated.h"

struct FBBBEquipFragment;
class UBBBEquipmentSystem;

/** 装备过渡运行数据 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipRuntimeData final : public UObject
{
    GENERATED_BODY()

public:
    /** @return 是否处于装备过渡 */
    bool IsEquipping() const
    {
        return bIsEquipping;
    }

private:
    friend struct FBBBEquipFragment;
    friend class UBBBEquipmentSystem;

    /** 是否处于装备过渡 */
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    bool bIsEquipping = false;

    /** 装备过渡开始时间 */
    float EquipStartTime = 0.0f;

    /** 装备过渡结束时间 */
    float EquipEndTime = 0.0f;

    /** 瞄准来源相对右手骨骼的固定变换 */
    FTransform AimSourceRightHandBoneSpace = FTransform::Identity;

    /** 左手目标相对右手骨骼的固定变换 */
    FTransform LeftHandTargetRightHandBoneSpace = FTransform::Identity;

    /** 瞄准来源固定变换是否有效 */
    bool bHasValidAimSource = false;

    /** 左手目标固定变换是否有效 */
    bool bHasValidLeftHandTarget = false;
};

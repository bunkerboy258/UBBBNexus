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

private:
    friend struct FBBBEquipFragment;
    friend class UBBBEquipmentSystem;

    /** 瞄准来源相对右手骨骼的固定变换 */
    FTransform AimSourceRightHandBoneSpace = FTransform::Identity;

    /** 瞄准来源固定变换是否有效 */
    bool bHasValidAimSource = false;

    /** 左手 IK 目标相对右手骨骼的静态 Socket 缓存 */
    FTransform LeftHandIKBaseTargetRightHandBoneSpace = FTransform::Identity;

    /** 左手 IK 目标 Socket 局部空间的运行时偏移 */
    FTransform LeftHandIKRuntimeSocketOffset = FTransform::Identity;

    /** 左手 IK 目标固定变换是否有效 */
    bool bHasValidLeftHandIKTarget = false;
};

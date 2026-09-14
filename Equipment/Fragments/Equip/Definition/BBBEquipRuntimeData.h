#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBEquipRuntimeData.generated.h"

struct FBBBEquipFragment;
class UBBBEquipmentSystem;
class UBBBEquipmentRuntimeData;

/** 装备过渡运行数据 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipRuntimeData final : public UObject
{
    GENERATED_BODY()

private:
    friend struct FBBBEquipFragment;
    friend class UBBBEquipmentRuntimeData;
    friend class UBBBEquipmentSystem;

    /** 开火领域提供的枪口与瞄准来源共用插槽 */
    FName MuzzleSocketName = NAME_None;

    /** 瞄准来源相对右手骨骼的固定变换 */
    FTransform AimSourceRightHandBoneSpace = FTransform::Identity;

    /** 瞄准来源固定变换是否有效 */
    bool bHasValidAimSource = false;

    /** 左手 IK 相对右手骨骼的固定位置 */
    FVector LeftHandIKOffsetRightHand = FVector::ZeroVector;

    /** 左手 IK 插槽在装备组件空间中的附加偏移 */
    FVector LeftHandIKSocketOffset = FVector::ZeroVector;

    /** 左手 IK 固定位置是否有效 */
    bool bHasValidLeftHandIKTarget = false;

};

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBBEquipRuntimeData.generated.h"

struct FBBBEquipFragment;
class UBBBEquipmentRuntimeData;
class FBBBEquipmentAnimationSystem;

/** 装备过渡运行数据 */
UCLASS(BlueprintType)
class ABBB_EVAC_API UBBBEquipRuntimeData final : public UObject
{
    GENERATED_BODY()

private:
    friend struct FBBBEquipFragment;
    friend class UBBBEquipmentRuntimeData;
    friend class FBBBEquipmentAnimationSystem;

    /** 开火领域提供的枪口与瞄准来源共用插槽 */
    FName MuzzleSocketName = NAME_None;

    /** 武器网格中的左手握持插槽 */
    FName LeftHandSocketName = TEXT("LeftHand");

    /** 左手插槽在武器组件空间中的附加位置偏移 */
    FVector LeftHandSocketOffset = FVector::ZeroVector;

    /** 左手 IK 目标在角色 hand_r 骨骼空间中的附加位置偏移 */
    FVector LeftHandIKOffset = FVector::ZeroVector;

    /** 瞄准来源相对右手骨骼的固定变换 */
    FTransform AimSourceRightHandBoneSpace = FTransform::Identity;

    /** 瞄准来源固定变换是否有效 */
    bool bHasValidAimSource = false;

};

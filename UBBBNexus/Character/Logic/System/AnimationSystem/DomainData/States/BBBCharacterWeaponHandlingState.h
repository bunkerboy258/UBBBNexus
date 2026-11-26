#pragma once

#include "CoreMinimal.h"

class UAnimSequence;
class UBBBEquipmentAnimInstance;

/** 与具体姿势图无关的持枪动画计算结果 */
struct FBBBCharacterWeaponHandlingState final
{
    /** 后坐力与呼吸摇摆合成角度 */
    FVector2D PoseOffsetAngles = FVector2D::ZeroVector;
    /** 当前姿态的瞄准跟随速度 */
    float AimFollowSpeed = 18.0f;
    /** 当前装备的瞄准最小求解距离 单位厘米 */
    float MinTargetDistance = 120.0f;
    /** 本次后震所属装备动画实例 */
    TWeakObjectPtr<UBBBEquipmentAnimInstance> RecoilSource;
    /** 最近消费的实际开火序号 */
    int32 RecoilFireSequence = 0;
    /** 持续衰减的后向震动强度 */
    float BackwardRecoilStrength = 0.0f;
    /** 装备配置持有的角色震动序列 */
    TWeakObjectPtr<UAnimSequence> RecoilAnimation;

private:
    friend struct FBBBCharacterAnimationDomainState;
    FBBBCharacterWeaponHandlingState() = default;
};

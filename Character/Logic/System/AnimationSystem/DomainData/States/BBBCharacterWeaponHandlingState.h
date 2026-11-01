#pragma once

#include "CoreMinimal.h"

class UAnimSequence;

/** 与具体姿势图无关的持枪动画计算结果 */
struct FBBBCharacterWeaponHandlingState final
{
    /** 后坐力与呼吸摇摆合成角度 */
    FVector2D PoseOffsetAngles = FVector2D::ZeroVector;
    /** 当前姿态的瞄准跟随速度 */
    float AimFollowSpeed = 18.0f;
    /** 距离实际开火的时间 */
    float RecoilTime = 1.0f;
    /** 当前姿态的后向震动强度 */
    float BackwardRecoilStrength = 0.0f;
    /** 装备配置持有的角色震动序列 */
    TWeakObjectPtr<UAnimSequence> RecoilAnimation;

private:
    friend struct FBBBCharacterAnimationDomainState;
    FBBBCharacterWeaponHandlingState() = default;
};

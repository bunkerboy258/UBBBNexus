#pragma once

#include "CoreMinimal.h"

class UBBBEquipmentAnimInstance;

/** 角色后坐力表现段的唯一运行状态 */
struct FBBBCharacterRecoilState final
{
    /** 所属装备动画实例 用于隔离切枪前后的表现段 */
    TWeakObjectPtr<UBBBEquipmentAnimInstance> Source;
    /** 已消费的开火事实序号 */
    int32 FireSequence = 0;
    /** 本段已成立的射击次数 */
    int32 ShotCount = 0;
    /** 未乘姿态倍率的当前二维角度 */
    FVector2D OffsetDegrees = FVector2D::ZeroVector;
    /** 自动射击段已结束 等待下一发实际开火后消费 */
    bool bStartNewSegment = true;

private:
    friend struct FBBBCharacterAnimationDomainState;
    FBBBCharacterRecoilState() = default;
};

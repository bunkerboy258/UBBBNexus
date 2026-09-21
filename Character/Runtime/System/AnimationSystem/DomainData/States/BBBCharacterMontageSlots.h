#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/AnimationSystem/DomainData/States/BBBCharacterMontageSlot.h"
#include "BBBCharacterMontageSlots.generated.h"

/** 角色动画图支持的五个固定蒙太奇槽位 */
USTRUCT()
struct FBBBCharacterMontageSlots final
{
    GENERATED_BODY()

    /** 全身槽位 */
    UPROPERTY(Transient)
    FBBBCharacterMontageSlot FullBody;

    /** 上半身槽位 */
    UPROPERTY(Transient)
    FBBBCharacterMontageSlot UpperBody;

    /** 瞄准前全身叠加槽位 */
    UPROPERTY(Transient)
    FBBBCharacterMontageSlot FullBodyAdditivePreAim;

    /** 上半身叠加槽位 */
    UPROPERTY(Transient)
    FBBBCharacterMontageSlot UpperBodyAdditive;

    /** 受击叠加槽位 */
    UPROPERTY(Transient)
    FBBBCharacterMontageSlot AdditiveHitReact;
};

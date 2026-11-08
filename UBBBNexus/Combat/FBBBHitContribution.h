#pragma once

#include "CoreMinimal.h"
#include "FBBBHitContribution.generated.h"

/** 一个来源对一个目标的一次战斗作用 不执行目标逻辑 */
USTRUCT()
struct ABBB_EVAC_API FBBBHitContribution final
{
    GENERATED_BODY()

    /** 普通伤害强度 */
    UPROPERTY()
    float AddDamage = 0;

    /** 接触骨骼 */
    UPROPERTY()
    FName Bone = NAME_None;

    /** 接触位置 */
    UPROPERTY()
    FVector Position = FVector::ZeroVector;

    /** 作用方向 */
    UPROPERTY()
    FVector Direction = FVector::ZeroVector;

    /** 耐久部位伤害强度 */
    UPROPERTY()
    float AddDurableDamage = 0;

    /** 踉跄强度 由目标决定触发概率 */
    UPROPERTY()
    float AddStagger = 0;

    /** 伤口强度 */
    UPROPERTY()
    float AddWound = 0;

    /** 世界空间冲量 */
    UPROPERTY()
    FVector AddImpulse = FVector::ZeroVector;

    /** 世界空间速度增量 单位厘米每秒 */
    UPROPERTY()
    FVector AddVelocity = FVector::ZeroVector;

    /** 冲击保持时间贡献 */
    UPROPERTY()
    float AddHoldSeconds = 0;

    /** 同一动作的分组身份 */
    UPROPERTY()
    FGuid AttackId = {};

    /** 来源实例身份 */
    UPROPERTY()
    FGuid SourceId = {};

    /** 贡献归属的玩家 */
    UPROPERTY()
    int32 PlayerId = INDEX_NONE;

    /** 来源位置 */
    UPROPERTY()
    FVector SourcePosition = FVector::ZeroVector;

    /** 是否提供来源位置 */
    UPROPERTY()
    bool bHasSourcePosition = false;

    /** 目标报告的接触部位编号 */
    UPROPERTY()
    uint8 Part = MAX_uint8;

    /** 接触表面法线 */
    UPROPERTY()
    FVector Normal = FVector::ZeroVector;

    /** 动作开始的同步时间 */
    UPROPERTY()
    double AttackTime = 0;

    /** 一次范围作用 */
    UPROPERTY()
    bool bRadial = false;

    /** 房主结算的世界来源 */
    UPROPERTY()
    bool bWorldSource = false;

    /** @return 命中贡献是否有限且至少含一项有效作用 */
    bool IsValid() const
    {
        return FMath::IsFinite(AddDamage) && AddDamage >= 0 && FMath::IsFinite(AddDurableDamage) && AddDurableDamage >= 0
            && FMath::IsFinite(AddStagger) && AddStagger >= 0 && FMath::IsFinite(AddWound) && AddWound >= 0
            && FMath::IsFinite(AddHoldSeconds) && AddHoldSeconds >= 0
            && FMath::IsFinite(AttackTime) && AttackTime >= 0 && !Position.ContainsNaN() && !Direction.ContainsNaN()
            && !SourcePosition.ContainsNaN() && !Normal.ContainsNaN() && !AddImpulse.ContainsNaN() && !AddVelocity.ContainsNaN()
            && (AddDamage > 0 || AddDurableDamage > 0 || AddHoldSeconds > 0 || !AddVelocity.IsNearlyZero()
                || !AddImpulse.IsNearlyZero() || AddWound > 0 || AddStagger > 0);
    }
};

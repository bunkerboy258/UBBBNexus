#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterHitRegion.h"
#include "BBBMonsterHitResult.generated.h"

/** 一个写入来源已成立的最近身体响应 不表达待结算伤害 */
USTRUCT()
struct FBBBMonsterHitResult final
{
    GENERATED_BODY()

    /** 同一来源在当前目标生命周期内的响应版本 */
    UPROPERTY()
    uint64 Sequence = 0;

    /** 已成立结果的服务器时间 */
    UPROPERTY()
    double Time = 0;

    /** 目标解释后的部位 */
    UPROPERTY()
    EBBBMonsterHitRegion Region = EBBBMonsterHitRegion::Torso;

    /** 目标解释后的接触位置 */
    UPROPERTY()
    FVector Position = FVector::ZeroVector;

    /** 目标解释后的受击方向 */
    UPROPERTY()
    FVector Direction = FVector::ZeroVector;

    /** 目标解释后的接触法线 */
    UPROPERTY()
    FVector Normal = FVector::ZeroVector;

    /** 本批已成立的物理冲量 */
    UPROPERTY()
    FVector Impulse = FVector::ZeroVector;

    /** 本批已成立的速度增量 */
    UPROPERTY()
    FVector Velocity = FVector::ZeroVector;

    /** @return 当前响应快照是否合法 */
    bool IsValid() const
    {
        return FMath::IsFinite(Time) && Region <= EBBBMonsterHitRegion::RightLeg
            && !Position.ContainsNaN() && !Direction.ContainsNaN() && !Normal.ContainsNaN()
            && !Impulse.ContainsNaN() && !Velocity.ContainsNaN();
    }

    bool operator==(const FBBBMonsterHitResult& Other) const
    {
        return Sequence == Other.Sequence && Time == Other.Time && Region == Other.Region
            && Position == Other.Position && Direction == Other.Direction && Normal == Other.Normal
            && Impulse == Other.Impulse && Velocity == Other.Velocity;
    }
};

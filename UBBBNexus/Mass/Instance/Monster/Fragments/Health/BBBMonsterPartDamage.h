#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitRegion.h"
#include "BBBMonsterPartDamage.generated.h"

/** 六个部位的累计有效伤害 当前结果不保存命中过程 */
USTRUCT()
struct FBBBMonsterPartDamage final
{
    GENERATED_BODY()

    /** 躯干累计伤害 */
    UPROPERTY()
    double Torso = 0.0;

    /** 头部累计伤害 */
    UPROPERTY()
    double Head = 0.0;

    /** 左臂累计伤害 */
    UPROPERTY()
    double LeftArm = 0.0;

    /** 右臂累计伤害 */
    UPROPERTY()
    double RightArm = 0.0;

    /** 左腿累计伤害 */
    UPROPERTY()
    double LeftLeg = 0.0;

    /** 右腿累计伤害 */
    UPROPERTY()
    double RightLeg = 0.0;

    /**
     * @param Part	部位编号
     * @return 对应累计分量
     */
    double Get(const EBBBMonsterHitRegion Part) const
    {
        switch (Part)
        {
            case EBBBMonsterHitRegion::Head:
                return Head;
            case EBBBMonsterHitRegion::LeftArm:
                return LeftArm;
            case EBBBMonsterHitRegion::RightArm:
                return RightArm;
            case EBBBMonsterHitRegion::LeftLeg:
                return LeftLeg;
            case EBBBMonsterHitRegion::RightLeg:
                return RightLeg;
            default:
                return Torso;
        }
    }

    /**
     * @param Part	部位编号
     * @param Amount	本轮新增有效伤害
     * @return 无
     */
    void Add(const EBBBMonsterHitRegion Part, const double Amount)
    {
        switch (Part)
        {
            case EBBBMonsterHitRegion::Head:
                Head += Amount;
                break;
            case EBBBMonsterHitRegion::LeftArm:
                LeftArm += Amount;
                break;
            case EBBBMonsterHitRegion::RightArm:
                RightArm += Amount;
                break;
            case EBBBMonsterHitRegion::LeftLeg:
                LeftLeg += Amount;
                break;
            case EBBBMonsterHitRegion::RightLeg:
                RightLeg += Amount;
                break;
            default:
                Torso += Amount;
                break;
        }
    }

    /**
     * @param Other	另一份当前累计结果
     * @return 无
     */
    void MergeMax(const FBBBMonsterPartDamage& Other)
    {
        Torso = FMath::Max(Torso, Other.Torso);
        Head = FMath::Max(Head, Other.Head);
        LeftArm = FMath::Max(LeftArm, Other.LeftArm);
        RightArm = FMath::Max(RightArm, Other.RightArm);
        LeftLeg = FMath::Max(LeftLeg, Other.LeftLeg);
        RightLeg = FMath::Max(RightLeg, Other.RightLeg);
    }

    /**
     * @param Other	已知累计结果
     * @return 是否有任一部位增加
     */
    bool Exceeds(const FBBBMonsterPartDamage& Other) const
    {
        return Torso > Other.Torso || Head > Other.Head || LeftArm > Other.LeftArm
            || RightArm > Other.RightArm || LeftLeg > Other.LeftLeg || RightLeg > Other.RightLeg;
    }

    /** @return 全部部位累计量 仅用于变化检测与诊断 */
    double Sum() const
    {
        return Torso + Head + LeftArm + RightArm + LeftLeg + RightLeg;
    }

    /** @return 所有分量是否有限且非负 */
    bool IsValid() const
    {
        return FMath::IsFinite(Torso) && Torso >= 0.0 && FMath::IsFinite(Head) && Head >= 0.0
            && FMath::IsFinite(LeftArm) && LeftArm >= 0.0 && FMath::IsFinite(RightArm) && RightArm >= 0.0
            && FMath::IsFinite(LeftLeg) && LeftLeg >= 0.0 && FMath::IsFinite(RightLeg) && RightLeg >= 0.0
            && FMath::IsFinite(Sum());
    }

    /**
     * @param Other	另一份当前累计结果
     * @return 各分量是否相同
     */
    bool operator==(const FBBBMonsterPartDamage& Other) const
    {
        return Torso == Other.Torso && Head == Other.Head && LeftArm == Other.LeftArm
            && RightArm == Other.RightArm && LeftLeg == Other.LeftLeg && RightLeg == Other.RightLeg;
    }
};

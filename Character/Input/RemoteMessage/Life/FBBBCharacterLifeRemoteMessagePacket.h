#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterLifeInputState.h"

/** 已成立的当前生命结果 只还原而不重演伤害 */
struct FBBBCharacterLifeRemoteMessagePacket final
{
    /** 同帧累计的生命阶段结果 */
    TArray<EBBBCharacterLifePhase> Phases;
    /** 对应阶段的生命值 */
    TArray<float> Healths;
    /** 结果版本 */
    TArray<uint64> Revisions;
    /** 已成立的最新命中序号 */
    TArray<uint64> HitSerials;
    /** 最新命中骨骼 */
    TArray<FName> Bones;
    /** 最新命中世界位置 */
    TArray<FVector> Positions;
    /** 最新命中世界方向 */
    TArray<FVector> Directions;

    /** 结果对应的倒地轮次 */
    TArray<uint64> DownedRevisions;
    /** 恢复阶段采用的蹲姿 */
    TArray<bool> RecoveryCrouched;

    /** @return 输入数据是否完整有效 */
    bool IsValid() const
    {
        if (Phases.IsEmpty() || Healths.Num() != Phases.Num() || Revisions.Num() != Phases.Num() ||
            HitSerials.Num() != Phases.Num() || Bones.Num() != Phases.Num() || Positions.Num() != Phases.Num() ||
            Directions.Num() != Phases.Num() || DownedRevisions.Num() != Phases.Num() ||
            RecoveryCrouched.Num() != Phases.Num())
        {
            return false;
        }
        for (int32 Index = 0; Index < Phases.Num(); ++Index)
        {
            if (Phases[Index] > EBBBCharacterLifePhase::Dead || Revisions[Index] == 0 ||
                !FMath::IsFinite(Healths[Index]) || Healths[Index] < 0.0f || Positions[Index].ContainsNaN() ||
                Directions[Index].ContainsNaN() ||
                ((Phases[Index] == EBBBCharacterLifePhase::Dead) != (Healths[Index] == 0.0f)))
            {
                return false;
            }
        }
        return true;
    }

    /**
     * @param Context	本次输入上下文
     * @return 是否允许应用当前输入
     */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return Context.bIsMirror;
    }

    /**
     * @param Context	本次输入上下文
     * @return 无
     */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        auto &Input = Context.LifeInputs;
        for (int32 Index = 0; Index < Revisions.Num(); ++Index)
        {
            if (Revisions[Index] <= Context.Life.Revision ||
                (Input.bHasResult && Revisions[Index] <= Input.ResultRevision))
            {
                continue;
            }
            Input.bHasResult = true;
            Input.ResultPhase = Phases[Index];
            Input.ResultHealth = Healths[Index];
            Input.ResultRevision = Revisions[Index];
            Input.ResultDownedRevision = DownedRevisions[Index];
            Input.bResultRecoveryCrouched = RecoveryCrouched[Index];
            Input.ResultHitSerial = HitSerials[Index];
            Input.ResultBone = Bones[Index];
            Input.ResultPosition = Positions[Index];
            Input.ResultDirection = Directions[Index];
        }
    }
};

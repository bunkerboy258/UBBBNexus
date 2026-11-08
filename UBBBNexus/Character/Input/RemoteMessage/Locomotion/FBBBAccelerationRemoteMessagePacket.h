#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 控制者已经产生的加速度结果 */
struct FBBBAccelerationRemoteMessagePacket final
{
    /** 同帧按到达顺序合并的结果版本 */
    TArray<uint64> Revisions;

    /** 与各版本对应的世界空间加速度 */
    TArray<FVector> Accelerations;

    /** 与各版本对应的已解析世界空间移动输入 仅用于事实还原 */
    TArray<FVector> MovementInputs;

    /** @return 数据是否完整且可用 */
    bool IsValid() const
    {
        if (Revisions.IsEmpty() || Revisions.Num() != Accelerations.Num()
            || Revisions.Num() != MovementInputs.Num())
        {
            return false;
        }
        for (int32 Index = 0; Index < Revisions.Num(); ++Index)
        {
            if (Revisions[Index] == 0 || Accelerations[Index].ContainsNaN()
                || Accelerations[Index].GetAbsMax() > 100000.0
                || MovementInputs[Index].ContainsNaN() || MovementInputs[Index].GetAbsMax() > 1.01)
            {
                return false;
            }
        }
        return true;
    }

    /** @param Context 本次输入上下文 @return 是否由镜像还原 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return Context.bIsMirror;
    }

    /** @param Context 本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        for (int32 Index = 0; Index < Revisions.Num(); ++Index)
        {
            if (Revisions[Index] > Context.Locomotion.AccelerationRevision)
            {
                Context.Locomotion.RestoredAcceleration = Accelerations[Index];
                Context.Locomotion.RestoredMovementInput = MovementInputs[Index];
                Context.Locomotion.AccelerationRevision = Revisions[Index];
            }
        }
    }
};

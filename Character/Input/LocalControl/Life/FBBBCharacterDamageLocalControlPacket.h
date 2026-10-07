#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterLifeInputState.h"

/** 已发生的独立命中 按提交顺序累计而不合并伤害 */
struct FBBBCharacterDamageLocalControlPacket final
{
    /** 每次命中的生命损失 */
    TArray<float> Damages;
    /** 每次命中的骨骼 */
    TArray<FName> Bones;
    /** 每次命中的世界位置 */
    TArray<FVector> Positions;
    /** 每次命中的世界冲击方向 */
    TArray<FVector> Directions;
    /** 命中来源的控制角色 允许环境来源为空 */
    TArray<TWeakObjectPtr<APawn>> Sources;

    /** @return 输入数据是否完整有效 */
    bool IsValid() const
    {
        if (Damages.IsEmpty() || Bones.Num() != Damages.Num() || Positions.Num() != Damages.Num() ||
            Directions.Num() != Damages.Num() || Sources.Num() != Damages.Num())
        {
            return false;
        }
        for (int32 Index = 0; Index < Damages.Num(); ++Index)
        {
            if (!FMath::IsFinite(Damages[Index]) || Damages[Index] <= 0.0f || Positions[Index].ContainsNaN() ||
                Directions[Index].ContainsNaN())
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
        return true;
    }

    /**
     * @param Context	本次输入上下文
     * @return 无
     */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.LifeInputs.Damages.Append(Damages);
        Context.LifeInputs.Bones.Append(Bones);
        Context.LifeInputs.Positions.Append(Positions);
        Context.LifeInputs.Directions.Append(Directions);
        Context.LifeInputs.Sources.Append(Sources);
    }
};

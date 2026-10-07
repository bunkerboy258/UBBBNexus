#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBCharacterDamageInboxState.h"

/** 独立命中投送消息 不包含扣血或阶段切换规则 */
struct FBBBCharacterDamageDeliveryRemoteMessagePacket final
{
    /** 每次独立伤害 */
    TArray<float> Damages;
    /** 命中骨骼 */
    TArray<FName> Bones;
    /** 命中世界位置 */
    TArray<FVector> Positions;
    /** 命中世界方向 */
    TArray<FVector> Directions;
    /** 合法连接的发送者 */
    TArray<TWeakObjectPtr<APawn>> Sources;
    /** 发送者在目标角色生命周期内的命中序号 */
    TArray<uint64> Sequences;

    /** @return 输入数据是否完整有效 */
    bool IsValid() const
    {
        if (Damages.IsEmpty() || Damages.Num() != Bones.Num() || Damages.Num() != Positions.Num() ||
            Damages.Num() != Directions.Num() || Damages.Num() != Sources.Num() || Damages.Num() != Sequences.Num())
        {
            return false;
        }
        for (int32 Index = 0; Index < Damages.Num(); ++Index)
        {
            if (!FMath::IsFinite(Damages[Index]) || Damages[Index] <= 0.0f || Sequences[Index] == 0 ||
                Positions[Index].ContainsNaN() || Directions[Index].ContainsNaN())
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
        Context.DamageInbox.Damages.Append(Damages);
        Context.DamageInbox.Bones.Append(Bones);
        Context.DamageInbox.Positions.Append(Positions);
        Context.DamageInbox.Directions.Append(Directions);
        Context.DamageInbox.Sources.Append(Sources);
        Context.DamageInbox.Sequences.Append(Sequences);
    }
};

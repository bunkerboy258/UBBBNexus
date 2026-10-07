#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentUseInputState.h"

/** 已成立的装备独立使用结果 */
struct FBBBCharacterEquipmentUseRemoteMessagePacket final
{
    /** 结果所属的持有实例 */
    TArray<uint64> Generations;
    /** 结果版本 */
    TArray<uint64> Revisions;
    /** 对应的使用许可 */
    TArray<bool> Usable;

    /** @return 输入数据是否完整有效 */
    bool IsValid() const
    {
        return !Generations.IsEmpty() && Generations.Num() == Revisions.Num() && Generations.Num() == Usable.Num() &&
               !Generations.Contains(uint64(0)) && !Revisions.Contains(uint64(0));
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
        Context.EquipmentUseInputs.Generations.Append(Generations);
        Context.EquipmentUseInputs.Revisions.Append(Revisions);
        Context.EquipmentUseInputs.Usable.Append(Usable);
    }
};

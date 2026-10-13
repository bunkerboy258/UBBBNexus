#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 已成立的角色装备持有关系 */
struct FBBBCharacterInteractionSelectionAuthorityFactPacket final
{
    /** 本帧累计的装备标识 空标识表示空手 */
    TArray<FName> ItemIds;

    /** 与装备标识对应的持有实例 */
    TArray<uint64> Generations;

    /** @return 字段数量与实例标识有效 */
    bool IsValid() const
    {
        return !ItemIds.IsEmpty() && ItemIds.Num() == Generations.Num()
            && !Generations.Contains(uint64(0));
    }

    /** @param Context	角色黑板 @return 是否有更新的持有关系 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return Generations.ContainsByPredicate([&Context](uint64 Value)
        {
            return Value > Context.InteractionSelection.ActiveGeneration;
        });
    }

    /** @param Context	角色黑板 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        for (int32 Index = 0; Index < Generations.Num(); ++Index)
        {
            if (Generations[Index] <= Context.InteractionSelection.ActiveGeneration
                || Generations[Index] < Context.InteractionSelection.PendingGeneration)
            {
                continue;
            }

            Context.InteractionSelection.PendingItemId = ItemIds[Index];
            Context.InteractionSelection.PendingGeneration = Generations[Index];
            Context.InteractionSelection.bHasInteractionRequest = true;
        }
    }
};

#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/BBBConsumable.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionUseState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/States/BBBCharacterItemOperationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 角色动画发布的通用物品Release通知 */
struct FBBBCharacterInteractionReleaseLocalControlPacket final
{
    /** 通知开始时对应的实例 */
    TArray<TWeakObjectPtr<UObject>> Recipients;
    /** 对应的持有代次 */
    TArray<uint64> Generations;
    /** 对应的使用序号 */
    TArray<int32> Tokens;

    /** @return 输入结构是否有效 */
    bool IsValid() const
    {
        return !Recipients.IsEmpty() && Recipients.Num() == Generations.Num()
            && Recipients.Num() == Tokens.Num();
    }
    /** @param Context	角色输入上下文 @return 是否拥有动作生成权限 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.Traversal.bTraversing && !Context.bIsMirror;

    }
    /** @param Context	角色输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        for (int32 Index = 0; Index < Recipients.Num(); ++Index)
        {
            if (Recipients[Index].Get() != Context.InteractionUse.Runtime.Get() ||
                Generations[Index] != static_cast<uint64>(Context.InteractionUse.UseSerial) ||
                Tokens[Index] != Context.InteractionUse.UseSerial || Tokens[Index] <= 0)
            {
                continue;
            }
            Context.InteractionInputs.ReleaseRecipients.Add(Recipients[Index]);
            Context.InteractionInputs.ReleaseGenerations.Add(Generations[Index]);
            Context.InteractionInputs.ReleaseTokens.Add(Tokens[Index]);
        }

    }
};

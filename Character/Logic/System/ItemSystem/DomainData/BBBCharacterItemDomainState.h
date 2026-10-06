#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/States/BBBCharacterItemInventoryState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/States/BBBCharacterItemBarState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/States/BBBCharacterItemOperationState.h"
#include "BBBCharacterItemDomainState.generated.h"

/** 角色物品领域全部状态的唯一持有者 */
USTRUCT(BlueprintType)
struct FBBBCharacterItemDomainState final
{
    GENERATED_BODY()

public:
    /** @return 真实背包只读状态 */
    const FBBBCharacterItemInventoryState &ReadItemInventoryState() const
    {
        return ItemInventoryState;
    }

    /** @return 快捷选择与目标主手物品只读状态 */
    const FBBBCharacterItemBarState &ReadItemBarState() const
    {
        return ItemBarState;
    }

    /** @return 待消费操作及完成结果只读状态 */
    const FBBBCharacterItemOperationState &ReadItemOperationState() const
    {
        return ItemOperationState;
    }

private:
    friend class FBBBCharacterItemSystem;
    friend class FBBBCharacterParseSystem;
    friend class FBBBCharacterItemAcquisitionProcessor;
    friend class FBBBCharacterItemInventoryProcessor;
    friend class FBBBCharacterItemBarProcessor;
    friend class FBBBCharacterItemSystemTest;

    /** 唯一真实物品存储 */
    UPROPERTY()
    FBBBCharacterItemInventoryState ItemInventoryState;

    /** 背包上层物品栏语义 */
    UPROPERTY()
    FBBBCharacterItemBarState ItemBarState;

    /** 输入与完成结果 */
    UPROPERTY()
    FBBBCharacterItemOperationState ItemOperationState;
};

#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentCommandState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentEventState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentInventoryState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "BBBCharacterEquipmentDomainState.generated.h"

class FBBBCharacterEquipmentActionProcessor;
class FBBBCharacterEquipmentController;
class FBBBCharacterEquipmentSelectionProcessor;
class FBBBCharacterParseSystem;

/** 角色装备领域状态的唯一持有者 */
USTRUCT(BlueprintType)
struct FBBBCharacterEquipmentDomainState final
{
    GENERATED_BODY()

public:
    /** @return 角色装备容器状态 */
    const FBBBCharacterEquipmentInventoryState &ReadEquipmentInventoryState() const
    {
        return EquipmentInventoryState;
    }

    /** @return 角色装备选择状态 */
    const FBBBCharacterEquipmentSelectionState &ReadEquipmentSelectionState() const
    {
        return EquipmentSelectionState;
    }

    /** @return 角色装备命令状态 */
    const FBBBCharacterEquipmentCommandState &ReadEquipmentCommandState() const
    {
        return EquipmentCommandState;
    }

    /** @return 角色装备事件状态 */
    const FBBBCharacterEquipmentEventState &ReadEquipmentEventState() const
    {
        return EquipmentEventState;
    }

private:
    friend class FBBBCharacterEquipmentActionProcessor;
    friend class FBBBCharacterEquipmentController;
    friend class FBBBCharacterEquipmentSelectionProcessor;
    friend class FBBBCharacterParseSystem;

    /** 角色装备容器状态 */
    UPROPERTY()
    FBBBCharacterEquipmentInventoryState EquipmentInventoryState;

    /** 角色装备选择状态 */
    UPROPERTY()
    FBBBCharacterEquipmentSelectionState EquipmentSelectionState;

    /** 角色装备命令状态 */
    UPROPERTY()
    FBBBCharacterEquipmentCommandState EquipmentCommandState;

    /** 角色装备事件状态 */
    UPROPERTY()
    FBBBCharacterEquipmentEventState EquipmentEventState;
};

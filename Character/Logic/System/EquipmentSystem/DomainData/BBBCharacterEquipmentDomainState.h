#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentInventoryState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "BBBCharacterEquipmentDomainState.generated.h"

class FBBBCharacterEquipmentSystem;
class FBBBCharacterEquipmentSelectionProcessor;
class FBBBCharacterEquipmentLifecycleProcessor;
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

private:
    friend class FBBBCharacterEquipmentSystem;
    friend class FBBBCharacterEquipmentSelectionProcessor;
    friend class FBBBCharacterEquipmentLifecycleProcessor;
    friend class FBBBCharacterParseSystem;

    /** 角色装备容器状态 */
    UPROPERTY()
    FBBBCharacterEquipmentInventoryState EquipmentInventoryState;

    /** 角色装备选择状态 */
    UPROPERTY()
    FBBBCharacterEquipmentSelectionState EquipmentSelectionState;


};

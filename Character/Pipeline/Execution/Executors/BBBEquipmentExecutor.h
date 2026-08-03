#pragma once

#include "CoreMinimal.h"

class UBBBEquipmentInstance;
struct FBBBCharacterEquipmentState;
struct FBBBCharacterItemInventoryState;
struct FBBBDecisionRuntimeData;

/** 根据获胜装备请求维护期望主手物品 */
class FBBBEquipmentExecutor final
{
public:

    void Update(
        FBBBDecisionRuntimeData &DecisionData,
        const FBBBCharacterItemInventoryState &InventoryState,
        FBBBCharacterEquipmentState &EquipmentState) const;

private:

    static UBBBEquipmentInstance *GetHotbarItem(
        const FBBBCharacterItemInventoryState &InventoryState,
        int32 HotbarSlot);
};

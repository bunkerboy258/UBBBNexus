
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterEquipmentState;
struct FBBBCharacterItemInventoryState;
struct FBBBDecisionRuntimeData;
struct FBBBItemInstance;

//根据获胜装备槽请求 维护期望主手物品
class FBBBEquipmentExecutor final
{
public:

    void Update(
        FBBBDecisionRuntimeData &DecisionData,
        const FBBBCharacterItemInventoryState &InventoryState,
        FBBBCharacterEquipmentState &EquipmentState) const;
private:
    static bool GetHotbarItem(
        const FBBBCharacterItemInventoryState &InventoryState,
        int32 HotbarSlot,
        FBBBItemInstance &OutItem);
};


#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterItemState;
struct FBBBItemInstance;

class FBBBCharacterItemStorage final
{
public:

    bool GetBackpackItem(const FBBBCharacterItemState &ItemState, int32 BackpackSlot, FBBBItemInstance &OutItem) const;

    bool GetHotbarItem(const FBBBCharacterItemState &ItemState, int32 HotbarSlot, FBBBItemInstance &OutItem) const;

    bool AddItem(FBBBCharacterItemState &ItemState, const FBBBItemInstance &ItemInstance) const;

    bool AddItemToSlot(FBBBCharacterItemState &ItemState, int32 SlotIndex, const FBBBItemInstance &ItemInstance) const;

    bool AssignHotbarItem(FBBBCharacterItemState &ItemState, int32 HotbarSlot, const FGuid &ItemInstanceId) const;
private:

    static bool FindEntryIndex(const FBBBCharacterItemState &ItemState, int32 SlotIndex, int32 &OutEntryIndex);

    static bool FindItemInstance(const FBBBCharacterItemState &ItemState, const FGuid &ItemInstanceId, FBBBItemInstance &OutItem);
};

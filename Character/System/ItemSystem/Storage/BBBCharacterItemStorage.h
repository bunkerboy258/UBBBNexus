
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterItemState;
class UBBBItemInstance;

class FBBBCharacterItemStorage final
{
public:

    UBBBItemInstance *GetBackpackItem(const FBBBCharacterItemState &ItemState, int32 BackpackSlot) const;

    UBBBItemInstance *GetHotbarItem(const FBBBCharacterItemState &ItemState, int32 HotbarSlot) const;

    bool AddItem(FBBBCharacterItemState &ItemState, UBBBItemInstance &ItemInstance) const;

    bool AddItemToSlot(FBBBCharacterItemState &ItemState, int32 SlotIndex, UBBBItemInstance &ItemInstance) const;

    bool AssignHotbarItem(FBBBCharacterItemState &ItemState, int32 HotbarSlot, const FGuid &ItemInstanceId) const;
private:

    static bool FindEntryIndex(const FBBBCharacterItemState &ItemState, int32 SlotIndex, int32 &OutEntryIndex);

    static UBBBItemInstance *FindItemInstance(const FBBBCharacterItemState &ItemState, const FGuid &ItemInstanceId);
};


#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterEquipmentState;
struct FBBBReloadNetworkPacket;

class ABBB_EVAC_API FBBBReloadRestoreProcessor final
{
public:

    void Update(
        const FBBBCharacterEquipmentState &EquipmentState,
        const FBBBReloadNetworkPacket &Packet) const;
};

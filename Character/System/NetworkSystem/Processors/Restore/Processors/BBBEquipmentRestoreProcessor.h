
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterEquipmentState;
struct FBBBEquipmentNetworkPacket;

class ABBB_EVAC_API FBBBEquipmentRestoreProcessor final
{
public:

    void Update(
        FBBBCharacterEquipmentState &EquipmentState,
        const FBBBEquipmentNetworkPacket &Packet) const;
};

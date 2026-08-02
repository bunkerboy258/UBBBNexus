
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterEquipmentState;
struct FBBBFireNetworkPacket;

class ABBB_EVAC_API FBBBFireRestoreProcessor final
{
public:

    void Update(
        const FBBBCharacterEquipmentState &EquipmentState,
        const FBBBFireNetworkPacket &Packet) const;
};

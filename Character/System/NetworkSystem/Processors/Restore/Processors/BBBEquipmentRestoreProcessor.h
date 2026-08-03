
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterEquipmentState;
struct FBBBEquipmentNetworkPacket;
class UBBBEquipmentCatalog;

class ABBB_EVAC_API FBBBEquipmentRestoreProcessor final
{
public:

    void Update(
        FBBBCharacterEquipmentState &EquipmentState,
        const FBBBEquipmentNetworkPacket &Packet,
        UBBBEquipmentCatalog &EquipmentCatalog,
        UObject &InstanceOuter) const;
};

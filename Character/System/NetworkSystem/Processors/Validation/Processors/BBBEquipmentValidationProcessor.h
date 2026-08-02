
#pragma once
#include "CoreMinimal.h"
struct FBBBEquipmentNetworkPacket;

class ABBB_EVAC_API FBBBEquipmentValidationProcessor final
{
public:
    bool Update(const FBBBEquipmentNetworkPacket &Packet) const;
};

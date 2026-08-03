
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterItemCommands;
struct FBBBReloadNetworkPacket;

class ABBB_EVAC_API FBBBReloadRestoreProcessor final
{
public:

    void Update(
        FBBBCharacterItemCommands &ItemCommands,
        const FBBBReloadNetworkPacket &Packet) const;
};

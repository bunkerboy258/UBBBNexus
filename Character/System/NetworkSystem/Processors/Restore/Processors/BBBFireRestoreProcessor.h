
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterItemCommands;
struct FBBBFireNetworkPacket;

class ABBB_EVAC_API FBBBFireRestoreProcessor final
{
public:

    void Update(
        FBBBCharacterItemCommands &ItemCommands,
        const FBBBFireNetworkPacket &Packet) const;
};

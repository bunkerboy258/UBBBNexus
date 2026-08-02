
#pragma once
#include "CoreMinimal.h"
class UBBBCharacterNetworkComponent;
struct FBBBCharacterItemEvents;

class ABBB_EVAC_API FBBBFireUploadProcessor final
{
public:

    void Update(
        const FBBBCharacterItemEvents &ItemEvents,
        UBBBCharacterNetworkComponent &NetworkComponent) const;
};

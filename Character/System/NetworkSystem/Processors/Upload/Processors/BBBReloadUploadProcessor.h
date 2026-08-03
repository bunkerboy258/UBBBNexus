
#pragma once
#include "CoreMinimal.h"
class UBBBCharacterNetworkComponent;
struct FBBBCharacterItemActionResults;

class ABBB_EVAC_API FBBBReloadUploadProcessor final
{
public:

    void Update(
        const FBBBCharacterItemActionResults &ActionResults,
        UBBBCharacterNetworkComponent &NetworkComponent) const;
};

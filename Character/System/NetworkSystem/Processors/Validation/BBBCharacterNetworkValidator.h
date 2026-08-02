
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Validation/Processors/BBBEquipmentValidationProcessor.h"
class UBBBCharacterNetworkComponent;
struct FBBBNetworkRuntimeData;

class ABBB_EVAC_API FBBBCharacterNetworkValidator final
{
public:

    void Update(
        FBBBNetworkRuntimeData &NetworkData,
        UBBBCharacterNetworkComponent &NetworkComponent) const;
private:
    FBBBEquipmentValidationProcessor EquipmentValidationProcessor;
};

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipmentActionFact.h"
#include "BBBNetworkFactEntryDefinition.generated.h"

USTRUCT()
struct FBBBNetworkFactEntryDefinition final : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY()
    FBBBEquipmentActionFact Fact;
};

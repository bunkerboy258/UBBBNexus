#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBEquipmentFactObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentEventState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"

void FBBBEquipmentFactObservationProcessor::Update(
    FBBBCharacterNetworkUpdateContext &Context) const
{
    for (const FBBBEquipmentActionFact &Fact : Context.EquipmentEventState.ActionEvents)
    {
        if (Context.NetworkIdentityState.bHasAuthority)
        {
            Context.NetworkComponent.ReplicateEquipmentFact(Fact);
            continue;
        }

        if (Context.NetworkIdentityState.bLocallyControlled)
        {
            Context.NetworkComponent.ServerSubmitEquipmentFact(Fact);
        }
    }
}

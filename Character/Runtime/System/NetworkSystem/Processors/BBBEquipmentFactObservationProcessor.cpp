#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/BBBEquipmentFactObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Runtime/Controller/EquipmentController/DomainData/States/BBBCharacterEquipmentEventState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"

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

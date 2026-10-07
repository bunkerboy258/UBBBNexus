#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBEquipmentObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBEquipmentNetworkObservationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Equipment/Base/Network/BBBEquipmentNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"

void FBBBEquipmentObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    auto *Character = Cast<ABBBCharacter>(Context.NetworkComponent.GetOwner());
    if (Character && Character->GetEquipmentNetworkComponent())
    {
        Character->GetEquipmentNetworkComponent()->DeliverPending();
    }

    if (Context.EquipmentObservation.Generation == Context.Equipment.ActiveGeneration)
    {
        return;
    }

    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateEquipment(Context.Equipment.ActiveEquipmentId, Context.Equipment.ActiveGeneration);
        Context.EquipmentObservation.Generation = Context.Equipment.ActiveGeneration;
    }

    if (!Context.NetworkIdentityState.bHasAuthority && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitEquipment(Context.Equipment.ActiveEquipmentId, Context.Equipment.ActiveGeneration);
        Context.EquipmentObservation.Generation = Context.Equipment.ActiveGeneration;
    }
}

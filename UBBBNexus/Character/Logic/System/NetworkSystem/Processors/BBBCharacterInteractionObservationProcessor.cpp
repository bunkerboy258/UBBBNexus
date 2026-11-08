#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBCharacterInteractionObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBCharacterInteractionNetworkObservationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/ExternalDomain/States/BBBCharacterNetworkIdentityState.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Base/Network/BBBEquipmentNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/BBBCharacter.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionUseState.h"

void FBBBCharacterInteractionObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    auto *Character = Cast<ABBBCharacter>(Context.NetworkComponent.GetOwner());
    if (Character && Character->GetEquipmentNetworkComponent())
    {
        Character->GetEquipmentNetworkComponent()->DeliverPending();
    }

    if (Character)
    {
        const auto &Fact = Context.Data.Interaction.ReadInteractionUseState().Fact;
        if (Fact.Revision > Context.InteractionObservation.UseRevision)
        {
            if (Context.NetworkIdentityState.bHasAuthority)
            {
                Context.NetworkComponent.ReplicateUse(Fact);
            }
            if (!Context.NetworkIdentityState.bHasAuthority && Context.NetworkIdentityState.bLocallyControlled)
            {
                Context.NetworkComponent.ServerSubmitUse(Fact);
            }
            Context.InteractionObservation.UseRevision = Fact.Revision;
        }
    }

    if (Context.InteractionObservation.Generation == Context.Interaction.ActiveGeneration) return;
    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateInteraction(Context.Interaction.ActiveItemId, Context.Interaction.ActiveGeneration);
        Context.InteractionObservation.Generation = Context.Interaction.ActiveGeneration;
    }
    if (!Context.NetworkIdentityState.bHasAuthority && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitInteraction(Context.Interaction.ActiveItemId, Context.Interaction.ActiveGeneration);
        Context.InteractionObservation.Generation = Context.Interaction.ActiveGeneration;
    }
}

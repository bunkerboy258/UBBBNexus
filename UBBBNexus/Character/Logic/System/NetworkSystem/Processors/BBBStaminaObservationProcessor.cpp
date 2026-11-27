#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBStaminaObservationProcessor.h"

#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"

void FBBBStaminaObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    auto &Observation = Context.Data.Network.StaminaObservationState;
    const auto &Stamina = Context.Data.Stamina.ReadStaminaState();
    if (!Stamina.bInitialized || Stamina.Revision <= Observation.Revision)
    {
        return;
    }
    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateStamina(
            Stamina.Current, Stamina.Maximum, Stamina.Revision, Stamina.bLockedUntilFull);
        Observation.Revision = Stamina.Revision;
    }
    if (!Context.NetworkIdentityState.bHasAuthority && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitStamina(
            Stamina.Current, Stamina.Maximum, Stamina.Revision, Stamina.bLockedUntilFull);
        Observation.Revision = Stamina.Revision;
    }
}

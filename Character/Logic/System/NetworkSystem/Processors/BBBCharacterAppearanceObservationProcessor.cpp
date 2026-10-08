#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBCharacterAppearanceObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"

void FBBBCharacterAppearanceObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    const auto &Snapshot = Context.Data.Appearance.ReadAppearanceSelectionState().Snapshot;
    auto &Observed = Context.Data.Network.AppearanceObservationState;
    if (!Snapshot.IsValid() || Snapshot.Revision == Observed.Revision)
    {
        return;
    }
    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateAppearance(Snapshot);
        Observed.Revision = Snapshot.Revision;
    }
    if (!Context.NetworkIdentityState.bHasAuthority && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitAppearance(Snapshot);
        Observed.Revision = Snapshot.Revision;
    }
}

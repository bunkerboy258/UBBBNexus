#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBCharacterRagdollObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"

void FBBBCharacterRagdollObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    const auto &Identity = Context.NetworkIdentityState;
    if (!Identity.bHasAuthority && !Identity.bLocallyControlled)
    {
        return;
    }
    const auto &Snapshot = Context.Data.PhysicalPresentation.ReadPhysicalPresentationState().RagdollSnapshot;
    auto &Observation = Context.Data.Network.RagdollObservationState;
    if (Snapshot.Revision == 0 || Snapshot.Revision <= Observation.Revision)
    {
        return;
    }
    const double Now = Context.WorldState.WorldTimeSeconds;
    // 当前结果最多每秒二十次 丢失后由后续快照收敛 不排队重演物理过程
    if (Identity.bLocallyControlled && Observation.LastSendTime >= 0.0
        && Now - Observation.LastSendTime < 0.05)
    {
        return;
    }
    if (Identity.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateRagdoll(Snapshot);
    }
    else
    {
        Context.NetworkComponent.ServerSubmitRagdoll(Snapshot);
    }
    Observation.Revision = Snapshot.Revision;
    Observation.LastSendTime = Now;
}

#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/Processors/BBBCharacterLifeObservationProcessor.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/Context/BBBCharacterNetworkUpdateContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/RuntimeData/BBBCharacterRuntimeData.h"
#include "BBBWork/UBBBNexus/Character/Network/BBBCharacterNetworkComponent.h"

void FBBBCharacterLifeObservationProcessor::Update(FBBBCharacterNetworkUpdateContext &Context) const
{
    auto &Observation = Context.Data.Network.LifeObservationState;
    const auto &Life = Context.Data.Life.ReadLifeState();
    const auto &Hit = Context.Data.Life.ReadHitState();
    if (!Life.bInitialized || Life.Revision <= Observation.Revision)
    {
        return;
    }
    if (Context.NetworkIdentityState.bHasAuthority)
    {
        Context.NetworkComponent.ReplicateLife(Life.Phase, Life.Health, Life.Revision, Hit.Serial, Hit.Bone,
                                               Hit.Position, Hit.Direction);
        Observation.Revision = Life.Revision;
    }
    if (!Context.NetworkIdentityState.bHasAuthority && Context.NetworkIdentityState.bLocallyControlled)
    {
        Context.NetworkComponent.ServerSubmitLife(Life.Phase, Life.Health, Life.Revision, Hit.Serial, Hit.Bone,
                                                  Hit.Position, Hit.Direction);
        Observation.Revision = Life.Revision;
    }
}

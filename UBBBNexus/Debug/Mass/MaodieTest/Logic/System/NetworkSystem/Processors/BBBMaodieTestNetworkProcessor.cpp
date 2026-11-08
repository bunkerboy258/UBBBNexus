#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/NetworkSystem/Processors/BBBMaodieTestNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Network/BBBMaodieTestNetworkComponent.h"
void FBBBMaodieTestNetworkProcessor::Update(ABBBMaodieTest &Actor, float DeltaSeconds, bool bCausal)
{
    if (!bCausal)
    {
        return;
    }
    auto &State = Actor.RuntimeData.Network.State;
    const auto &Snapshot = Actor.RuntimeData.Behavior.ReadMaodieTestBehaviorState().Snapshot;
    if (Snapshot.Phase != State.Published.Phase || Snapshot.HissSerial != State.Published.HissSerial
        || Snapshot.bHasLookTarget != State.Published.bHasLookTarget
        || FVector::DistSquared(Snapshot.LookTarget, State.Published.LookTarget) >= 25.0f)
    {
        Actor.Network->Publish(Snapshot);
        State.Published = Snapshot;
    }
}

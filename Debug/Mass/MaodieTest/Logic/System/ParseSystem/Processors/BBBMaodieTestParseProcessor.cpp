#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/ParseSystem/Processors/BBBMaodieTestParseProcessor.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
void FBBBMaodieTestParseProcessor::Update(ABBBMaodieTest &Actor, float DeltaSeconds, bool bCausal)
{
    auto &State = Actor.RuntimeData.Parse.State;
    auto &Behavior = Actor.RuntimeData.Behavior.State;
    if (State.bLocalHitsActive && State.LocalHits.IsValid() && State.LocalHits.CanApply(bCausal))
    {
        State.LocalHits.Apply(Behavior.PendingShooters);
    }
    if (State.bRemoteHitsActive && State.RemoteHits.IsValid() && State.RemoteHits.CanApply(bCausal))
    {
        State.RemoteHits.Apply(Behavior.PendingShooters);
    }
    State.LocalHits.Shooters.Reset();
    State.RemoteHits.Shooters.Reset();
    State.bLocalHitsActive = false;
    State.bRemoteHitsActive = false;
    if (!State.bActive)
    {
        return;
    }
    const FBBBMaodieTestSnapshotAuthorityFactPacket Packet{State.Pending};
    if (Packet.IsValid() && Packet.CanApply(bCausal))
    {
        Packet.Apply(Behavior.Snapshot);
    }
    State.bActive = false;
}

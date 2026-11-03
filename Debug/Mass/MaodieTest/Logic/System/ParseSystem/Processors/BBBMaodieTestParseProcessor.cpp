#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/Logic/System/ParseSystem/Processors/BBBMaodieTestParseProcessor.h"
#include "BBBWork/UBBBNexus/Debug/Mass/MaodieTest/BBBMaodieTest.h"
void FBBBMaodieTestParseProcessor::Update(ABBBMaodieTest &Actor, float DeltaSeconds, bool bCausal)
{
    auto &State = Actor.RuntimeData.Parse.State;
    if (!State.bActive)
    {
        return;
    }
    const FBBBMaodieTestSnapshotAuthorityFactPacket Packet{State.Pending};
    if (Packet.IsValid() && Packet.CanApply(bCausal))
    {
        Packet.Apply(Actor.RuntimeData.Behavior.State.Snapshot);
    }
    State.bActive = false;
}

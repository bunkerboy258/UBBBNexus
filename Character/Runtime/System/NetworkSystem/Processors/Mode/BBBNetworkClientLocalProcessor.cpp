#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/Mode/BBBNetworkClientLocalProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkSystem.h"

void FBBBNetworkClientLocalProcessor::Update(FBBBCharacterNetworkSystem &System) const
{
    System.ObserveFacts();
}

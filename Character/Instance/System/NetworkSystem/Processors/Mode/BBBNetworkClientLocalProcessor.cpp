#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Mode/BBBNetworkClientLocalProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/BBBCharacterNetworkSystem.h"

void FBBBNetworkClientLocalProcessor::Update(FBBBCharacterNetworkSystem &System) const
{
    System.ObserveFacts();
}

#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/Processors/Mode/BBBNetworkAuthorityLocalProcessor.h"
#include "BBBWork/UBBBNexus/Character/Instance/System/NetworkSystem/BBBCharacterNetworkSystem.h"

void FBBBNetworkAuthorityLocalProcessor::Update(FBBBCharacterNetworkSystem &System) const
{
    System.ObserveFacts();
}

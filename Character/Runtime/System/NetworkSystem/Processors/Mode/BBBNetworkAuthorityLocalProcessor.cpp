#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/Processors/Mode/BBBNetworkAuthorityLocalProcessor.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/NetworkSystem/BBBCharacterNetworkSystem.h"

void FBBBNetworkAuthorityLocalProcessor::Update(FBBBCharacterNetworkSystem &System) const
{
    System.ObserveFacts();
}


#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBFireUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/Results/BBBCharacterItemActionResults.h"

void FBBBFireUploadProcessor::Update(
    const FBBBCharacterItemActionResults &ActionResults,
    UBBBCharacterNetworkComponent &NetworkComponent) const
{

    for (int32 Index = 0; Index < ActionResults.GetSuccessfulFireCount(); ++Index)
    {

        FBBBFireNetworkPacket Packet;

        NetworkComponent.SendFirePacket(Packet);
    }
}

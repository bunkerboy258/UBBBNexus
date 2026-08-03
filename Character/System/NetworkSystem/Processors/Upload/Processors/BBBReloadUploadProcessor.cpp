
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBReloadUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Context/BBBCharacterItemActionResults.h"

void FBBBReloadUploadProcessor::Update(
    const FBBBCharacterItemActionResults &ActionResults,
    UBBBCharacterNetworkComponent &NetworkComponent) const
{
    for (int32 Index = 0; Index < ActionResults.GetSuccessfulReloadCount(); ++Index)
    {
        FBBBReloadNetworkPacket Packet;

        NetworkComponent.SendReloadPacket(Packet);
    }
}


#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBReloadUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemEvents.h"

void FBBBReloadUploadProcessor::Update(
    const FBBBCharacterItemEvents &ItemEvents,
    UBBBCharacterNetworkComponent &NetworkComponent) const
{
    for (int32 Index = 0; Index < ItemEvents.GetReloadEventCount(); ++Index)
    {
        FBBBReloadNetworkPacket Packet;

        NetworkComponent.SendReloadPacket(Packet);
    }
}

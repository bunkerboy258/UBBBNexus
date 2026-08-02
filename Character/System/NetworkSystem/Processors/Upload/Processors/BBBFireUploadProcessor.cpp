
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBFireUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemEvents.h"

void FBBBFireUploadProcessor::Update(
    const FBBBCharacterItemEvents &ItemEvents,
    UBBBCharacterNetworkComponent &NetworkComponent) const
{

    for (int32 Index = 0; Index < ItemEvents.GetFireEventCount(); ++Index)
    {

        FBBBFireNetworkPacket Packet;

        NetworkComponent.SendFirePacket(Packet);
    }
}

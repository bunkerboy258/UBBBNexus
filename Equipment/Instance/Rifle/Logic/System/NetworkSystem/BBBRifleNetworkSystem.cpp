#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/NetworkSystem/BBBRifleNetworkSystem.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/NetworkSystem/Processors/BBBRifleNetworkProcessor.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/AuthorityFact/Action/FBBBRifleActionStateAuthorityFactPacket.h"

void FBBBRifleNetworkSystem::Update(FBBBRifleUpdateContext &Context)
{
    FBBBRifleNetworkProcessor::Update(Context);
}

bool FBBBRifleNetworkSystem::DecodeAuthorityFact(
    const FBBBRifleRuntimeData &Data,
    const TArray<uint8> &Payload,
    FBBBRifleActionStateAuthorityFactPacket &Packet)
{
    return FBBBRifleNetworkProcessor::Decode(Data, Payload, Packet);
}

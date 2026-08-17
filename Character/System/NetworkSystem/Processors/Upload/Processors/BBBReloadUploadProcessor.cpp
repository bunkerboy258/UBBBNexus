
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBReloadUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Results/BBBCharacterEquipmentResults.h"

void FBBBReloadUploadProcessor::Update(
    const FBBBCharacterEquipmentResults &EquipmentResults,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{
    for (int32 Index = 0; Index < EquipmentResults.GetSuccessfulReloadCount(); ++Index)
    {
        FBBBReloadNetworkPacket Packet;

        NetworkSystem.SubmitReloadPacket(MoveTemp(Packet));
    }
}


#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Upload/Processors/BBBFireUploadProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkSystem.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Results/BBBCharacterEquipmentResults.h"

void FBBBFireUploadProcessor::Update(
    const FBBBCharacterEquipmentResults &EquipmentResults,
    FBBBCharacterNetworkSystem &NetworkSystem) const
{

    for (int32 Index = 0; Index < EquipmentResults.GetSuccessfulFireCount(); ++Index)
    {

        FBBBFireNetworkPacket Packet;

        NetworkSystem.SubmitFirePacket(MoveTemp(Packet));
    }
}

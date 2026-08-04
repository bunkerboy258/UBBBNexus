
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBFireRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBFireNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/Commands/BBBCharacterEquipmentCommands.h"

void FBBBFireRestoreProcessor::Update(
    FBBBCharacterEquipmentCommands &EquipmentCommands,
    const FBBBFireNetworkPacket &) const
{

    EquipmentCommands.SubmitFirePresentation();
}


#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBFireRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBFireNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Context/BBBCharacterItemCommands.h"

void FBBBFireRestoreProcessor::Update(
    FBBBCharacterItemCommands &ItemCommands,
    const FBBBFireNetworkPacket &) const
{

    ItemCommands.SubmitFirePresentation();
}

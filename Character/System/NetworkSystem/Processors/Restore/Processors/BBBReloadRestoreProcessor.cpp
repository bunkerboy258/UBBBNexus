
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBReloadRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBReloadNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/Commands/BBBCharacterItemCommands.h"

void FBBBReloadRestoreProcessor::Update(
    FBBBCharacterItemCommands &ItemCommands,
    const FBBBReloadNetworkPacket &) const
{

    ItemCommands.SubmitReloadPresentation();
}

#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBEquipmentRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/EquipmentSystem/Definition/States/BBBCharacterEquipmentStates.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Equipment/Base/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

void FBBBEquipmentRestoreProcessor::Update(
    FBBBCharacterEquipmentState &EquipmentState,
    const FBBBEquipmentNetworkPacket &Packet,
    UBBBEquipmentCatalog &EquipmentCatalog,
    UObject &InstanceOuter) const
{
    UBBBEquipmentDefinition *Definition = EquipmentCatalog.FindDefinition(Packet.EquipmentHandle);
    if (!ensureMsgf(Definition, TEXT("[UBBBC]Equipment restore handle has no matching definition")))
    { return; }

    UBBBEquipmentInstance *CurrentInstance = EquipmentState.DesiredMainHandInstance;
    if (CurrentInstance && CurrentInstance->GetDefinition() == Definition)
    { return; }

    UBBBEquipmentInstance *RestoredInstance = UBBBEquipmentInstance::Create(InstanceOuter, *Definition);
    if (!ensureMsgf(RestoredInstance, TEXT("[UBBBC]Restored equipment instance creation failed")))
    { return; }

    EquipmentState.DesiredMainHandInstance = RestoredInstance;
}

#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBEquipmentRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/BBBCharacterItemState.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentDefinition.h"
#include "BBBWork/UBBBNexus/Item/Base/Equipment/BBBEquipmentInstance.h"
#include "BBBWork/UBBBNexus/Item/CatalogSystem/BBBEquipmentCatalog.h"

void FBBBEquipmentRestoreProcessor::Update(
    FBBBCharacterEquipmentState &EquipmentState,
    const FBBBEquipmentNetworkPacket &Packet,
    UBBBEquipmentCatalog &EquipmentCatalog,
    UObject &InstanceOuter) const
{
    UBBBEquipmentDefinition *Definition = EquipmentCatalog.FindDefinition(Packet.EquipmentHandle);
    if (!ensureMsgf(Definition, TEXT("[UBBBC]Equipment restore handle has no matching definition")))
    {
        return;
    }

    UBBBEquipmentInstance *CurrentInstance = EquipmentState.DesiredMainHandInstance;
    if (CurrentInstance && CurrentInstance->GetDefinition() == Definition)
    {
        return;
    }

    UBBBEquipmentInstance *MirrorInstance = UBBBEquipmentInstance::CreateMirror(InstanceOuter, *Definition);
    if (!ensureMsgf(MirrorInstance, TEXT("[UBBBC]Equipment mirror instance creation failed")))
    {
        return;
    }

    EquipmentState.DesiredMainHandInstance = MirrorInstance;
}

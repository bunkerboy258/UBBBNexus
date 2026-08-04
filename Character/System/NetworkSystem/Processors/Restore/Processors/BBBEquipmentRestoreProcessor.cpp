#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Restore/Processors/BBBEquipmentRestoreProcessor.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/ItemSystem/Definition/States/BBBCharacterItemStates.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemDefinition.h"
#include "BBBWork/UBBBNexus/Item/Base/BBBItemInstance.h"
#include "BBBWork/UBBBNexus/Item/CatalogSystem/BBBEquipmentCatalog.h"

void FBBBEquipmentRestoreProcessor::Update(
    FBBBCharacterEquipmentState &EquipmentState,
    const FBBBEquipmentNetworkPacket &Packet,
    UBBBEquipmentCatalog &EquipmentCatalog,
    UObject &InstanceOuter) const
{
    UBBBItemDefinition *Definition = EquipmentCatalog.FindDefinition(Packet.EquipmentHandle);
    if (!ensureMsgf(Definition, TEXT("[UBBBC]Equipment restore handle has no matching definition")))
    { return; }

    UBBBItemInstance *CurrentInstance = EquipmentState.DesiredMainHandInstance;
    if (CurrentInstance && CurrentInstance->GetDefinition() == Definition)
    { return; }

    UBBBItemInstance *MirrorInstance = UBBBItemInstance::CreateMirror(InstanceOuter, *Definition);
    if (!ensureMsgf(MirrorInstance, TEXT("[UBBBC]Equipment mirror instance creation failed")))
    { return; }

    EquipmentState.DesiredMainHandInstance = MirrorInstance;
}

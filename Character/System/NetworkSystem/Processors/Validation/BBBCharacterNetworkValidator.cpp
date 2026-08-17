
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Validation/BBBCharacterNetworkValidator.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

//校验网络包
TArray<FBBBEquipmentNetworkPacket> FBBBCharacterNetworkValidator::Update(
    FBBBNetworkRuntimeData &NetworkData,
    const UBBBEquipmentCatalog &EquipmentCatalog) const
{
    TArray<FBBBEquipmentNetworkPacket> PendingPackets = NetworkData.ValidationEquipmentPackets();
    TArray<FBBBEquipmentNetworkPacket> ValidPackets;
    ValidPackets.Reserve(PendingPackets.Num());

    for (FBBBEquipmentNetworkPacket &Packet : PendingPackets)
    {
        if (!EquipmentValidationProcessor.Update(Packet, EquipmentCatalog))
        {
            UE_LOG(LogTemp, Warning, TEXT("Rejected invalid equipment network packet"));
            continue;
        }

        ValidPackets.Add(MoveTemp(Packet));
    }

    return ValidPackets;
}

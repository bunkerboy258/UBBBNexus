
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Validation/BBBCharacterNetworkValidator.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/BBBCharacterNetworkComponent.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/BBBNetworkRuntimeData.h"

//校验网络包
void FBBBCharacterNetworkValidator::Update(
    FBBBNetworkRuntimeData &NetworkData,
    UBBBCharacterNetworkComponent &NetworkComponent) const
{

    TArray<FBBBEquipmentNetworkPacket> PendingPackets = NetworkData.ValidationEquipmentPackets();

    for (const FBBBEquipmentNetworkPacket &Packet : PendingPackets)
    {
        if (!EquipmentValidationProcessor.Update(Packet))
        {
            UE_LOG(LogTemp, Warning, TEXT("Rejected invalid equipment network packet"));
            continue;
        }
        //校验通过就广播
        NetworkComponent.MulticastEquipmentPacket(Packet);
    }
}

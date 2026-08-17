
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Definition/Packets/BBBEquipmentNetworkPacket.h"
#include "BBBWork/UBBBNexus/Character/System/NetworkSystem/Processors/Validation/Processors/BBBEquipmentValidationProcessor.h"
class UBBBEquipmentCatalog;
struct FBBBNetworkRuntimeData;

class ABBB_EVAC_API FBBBCharacterNetworkValidator final
{
public:

    /**
     * 逐帧取出待校验装备Packet并返回通过者
     * @param NetworkData		网络运行时数据
     * @param EquipmentCatalog	装备目录
     * @return 通过权威校验的装备Packet
     */
    TArray<FBBBEquipmentNetworkPacket> Update(
        FBBBNetworkRuntimeData &NetworkData,
        const UBBBEquipmentCatalog &EquipmentCatalog) const;
private:
    FBBBEquipmentValidationProcessor EquipmentValidationProcessor;
};

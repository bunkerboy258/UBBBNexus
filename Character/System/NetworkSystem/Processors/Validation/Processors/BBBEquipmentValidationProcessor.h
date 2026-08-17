
#pragma once
#include "CoreMinimal.h"
struct FBBBEquipmentNetworkPacket;
class UBBBEquipmentCatalog;

class ABBB_EVAC_API FBBBEquipmentValidationProcessor final
{
public:
    /**
     * 校验装备Packet是否指向目录内的有效装备
     * @param Packet            装备网络Packet
     * @param EquipmentCatalog  装备目录
     * @return Packet是否通过校验
     */
    bool Update(
        const FBBBEquipmentNetworkPacket &Packet,
        const UBBBEquipmentCatalog &EquipmentCatalog) const;
};

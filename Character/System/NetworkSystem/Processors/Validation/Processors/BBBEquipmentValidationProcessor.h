
#pragma once
#include "CoreMinimal.h"
struct FBBBEquipmentNetworkPacket;

class ABBB_EVAC_API FBBBEquipmentValidationProcessor final
{
public:
    /**
     * 校验装备Packet是否携带有效装备句柄
     * @param Packet	装备网络Packet
     * @return Packet是否通过校验
     */
    bool Update(const FBBBEquipmentNetworkPacket &Packet) const;
};

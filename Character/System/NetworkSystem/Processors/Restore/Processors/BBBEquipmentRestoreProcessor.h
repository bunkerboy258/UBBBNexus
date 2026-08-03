
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterEquipmentState;
struct FBBBEquipmentNetworkPacket;
class UBBBEquipmentCatalog;

class ABBB_EVAC_API FBBBEquipmentRestoreProcessor final
{
public:

    /**
     * 根据装备Packet创建镜像装备实例并设为期望主手装备
     * @param EquipmentState	角色装备状态
     * @param Packet	装备网络Packet
     * @param EquipmentCatalog	装备目录
     * @param InstanceOuter	镜像装备实例生命周期所有者
     */
    void Update(
        FBBBCharacterEquipmentState &EquipmentState,
        const FBBBEquipmentNetworkPacket &Packet,
        UBBBEquipmentCatalog &EquipmentCatalog,
        UObject &InstanceOuter) const;
};

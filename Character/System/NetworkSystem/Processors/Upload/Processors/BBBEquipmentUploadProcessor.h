
#pragma once
#include "CoreMinimal.h"
//封装UBBB角色网络组件的数据与行为
class UBBBCharacterNetworkComponent;
struct FBBBCharacterEquipmentState;
struct FBBBNetworkRuntimeData;

class ABBB_EVAC_API FBBBEquipmentUploadProcessor final
{
public:

    /**
     * 当前主手装备变化时上传装备Packet
     * @param EquipmentState	角色装备状态
     * @param NetworkData	网络运行时数据
     * @param NetworkComponent	角色网络组件
     */
    void Update(
        const FBBBCharacterEquipmentState &EquipmentState,
        FBBBNetworkRuntimeData &NetworkData,
        UBBBCharacterNetworkComponent &NetworkComponent) const;
};

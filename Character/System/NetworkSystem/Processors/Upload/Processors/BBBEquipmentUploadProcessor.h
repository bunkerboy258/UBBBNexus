
#pragma once
#include "CoreMinimal.h"
class FBBBCharacterNetworkSystem;
struct FBBBCharacterEquipmentState;
struct FBBBNetworkRuntimeData;

class ABBB_EVAC_API FBBBEquipmentUploadProcessor final
{
public:

    /**
     * 当前主手装备变化时上传装备Packet
     * @param EquipmentState	角色装备状态
     * @param NetworkData	网络运行时数据
     * @param NetworkSystem	角色网络系统
     */
    void Update(
        const FBBBCharacterEquipmentState &EquipmentState,
        FBBBNetworkRuntimeData &NetworkData,
        FBBBCharacterNetworkSystem &NetworkSystem) const;
};

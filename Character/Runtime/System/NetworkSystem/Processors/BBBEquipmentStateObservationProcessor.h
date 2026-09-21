
#pragma once
#include "CoreMinimal.h"
struct FBBBCharacterNetworkUpdateContext;

class ABBB_EVAC_API FBBBEquipmentStateObservationProcessor final
{
public:

    /**
     * 当前主手装备变化时上传装备Packet
     * @param EquipmentState	角色装备状态
     * @param NetworkData	网络运行时数据
     * @param NetworkSystem	角色网络系统
     */
    /**
     * 当前主手装备变化时上传装备状态
     * @param Context 本次网络更新上下文
     * @return 无
     */
    void Update(FBBBCharacterNetworkUpdateContext &Context) const;
};

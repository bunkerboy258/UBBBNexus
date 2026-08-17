
#pragma once
#include "CoreMinimal.h"
class FBBBCharacterNetworkSystem;
struct FBBBCharacterEquipmentResults;

class ABBB_EVAC_API FBBBFireUploadProcessor final
{
public:

    /**
     * 按本帧成功开火次数上传开火Packet
     * @param EquipmentResults	装备动作结果
     * @param NetworkSystem	角色网络系统
     */
    void Update(
        const FBBBCharacterEquipmentResults &EquipmentResults,
        FBBBCharacterNetworkSystem &NetworkSystem) const;
};

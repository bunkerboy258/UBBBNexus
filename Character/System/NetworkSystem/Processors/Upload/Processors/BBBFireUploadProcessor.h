
#pragma once
#include "CoreMinimal.h"
class UBBBCharacterNetworkComponent;
struct FBBBCharacterItemActionResults;

class ABBB_EVAC_API FBBBFireUploadProcessor final
{
public:

    /**
     * 按本帧成功开火次数上传开火Packet
     * @param ActionResults	装备动作结果
     * @param NetworkComponent	角色网络组件
     */
    void Update(
        const FBBBCharacterItemActionResults &ActionResults,
        UBBBCharacterNetworkComponent &NetworkComponent) const;
};

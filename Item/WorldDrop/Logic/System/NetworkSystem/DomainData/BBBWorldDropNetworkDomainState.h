#pragma once
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/NetworkSystem/DomainData/States/BBBWorldDropNetworkState.h"
#include "BBBWorldDropNetworkDomainState.generated.h"
/** 掉落物网络发送领域的唯一状态持有者 */
USTRUCT()
struct FBBBWorldDropNetworkDomainState final
{
    GENERATED_BODY()
public:
    /** @return 网络发布和交付发送基准 */
    const FBBBWorldDropNetworkState &ReadWorldDropNetworkState() const
    {
        return NetworkState;
    }
private:
    friend class FBBBWorldDropNetworkProcessor;
    /** 只由网络处理器维护的发送基准 */
    UPROPERTY()
    FBBBWorldDropNetworkState NetworkState;
};

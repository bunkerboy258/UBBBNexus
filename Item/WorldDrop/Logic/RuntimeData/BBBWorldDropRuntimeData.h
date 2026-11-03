#pragma once
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/ParseSystem/DomainData/BBBWorldDropParseDomainState.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/DropSystem/DomainData/BBBWorldDropDomainState.h"
#include "BBBWork/UBBBNexus/Item/WorldDrop/Logic/System/NetworkSystem/DomainData/BBBWorldDropNetworkDomainState.h"
#include "BBBWorldDropRuntimeData.generated.h"
/** 世界掉落物的唯一运行时黑板 */
USTRUCT()
struct FBBBWorldDropRuntimeData final
{
    GENERATED_BODY()
    /** 外部输入解析领域 */
    FBBBWorldDropParseDomainState Parse;
    /** 掉落与领取业务领域 */
    UPROPERTY()
    FBBBWorldDropDomainState Drop;
    /** 网络结果发送领域 */
    UPROPERTY()
    FBBBWorldDropNetworkDomainState Network;
};

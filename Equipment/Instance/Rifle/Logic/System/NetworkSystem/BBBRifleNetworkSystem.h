#pragma once

#include "CoreMinimal.h"

struct FBBBRifleUpdateContext;
struct FBBBRifleRuntimeData;
struct FBBBRifleActionStateAuthorityFactPacket;

/** 步枪Network系统的固定调度入口 */
class FBBBRifleNetworkSystem final
{
public:
    /** @param Context	本次装备更新上下文 @return 无 */
    static void Update(FBBBRifleUpdateContext &Context);

    /**
     * 解码网络组件收到的步枪事实载荷
     * @param Data		步枪运行时数据
     * @param Payload	网络事实载荷
     * @param Packet	解码后的步枪事实
     * @return 是否解码成功
     */
    static bool DecodeAuthorityFact(
        const FBBBRifleRuntimeData &Data,
        const TArray<uint8> &Payload,
        FBBBRifleActionStateAuthorityFactPacket &Packet);
};

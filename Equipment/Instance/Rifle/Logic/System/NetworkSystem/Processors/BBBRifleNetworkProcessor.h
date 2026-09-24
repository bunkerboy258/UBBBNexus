#pragma once

#include "CoreMinimal.h"

struct FBBBRifleRuntimeData;
struct FBBBRifleUpdateContext;

/** 步枪当前状态的编码 发布与接收校验 */
class FBBBRifleNetworkProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBRifleUpdateContext &Context);

    /** @param Data	步枪运行时数据 @param Payload	输出编码 @return 无 */
    static void Encode(const FBBBRifleRuntimeData &Data, TArray<uint8> &Payload);

    /** @param Data	步枪运行时数据 @param Payload	网络编码 @return 是否接受 */
    static bool Submit(FBBBRifleRuntimeData &Data, const TArray<uint8> &Payload);
};

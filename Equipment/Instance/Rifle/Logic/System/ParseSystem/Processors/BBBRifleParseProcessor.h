#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/RuntimeData/BBBRifleRuntimeData.h"

struct FBBBRifleUpdateContext;

/** 固定输入存储与来源检查 */
class FBBBRifleParseProcessor final
{
public:
    /** @param Context	本次更新上下文 @return 无 */
    static void Update(FBBBRifleUpdateContext &Context);

    /** @param Data	步枪运行时数据 @return 无 */
    static void Clear(FBBBRifleRuntimeData &Data);

    /** @param Data	步枪运行时数据 @param Packet	待消费请求 @return 无 */
    static void Submit(FBBBRifleRuntimeData &Data, const FBBBRifleEquipPacket &Packet);

    /** @param Data	步枪运行时数据 @param Packet	待消费请求 @return 无 */
    static void Submit(FBBBRifleRuntimeData &Data, const FBBBRifleFirePacket &Packet);

    /** @param Data	步枪运行时数据 @param Packet	待消费请求 @return 无 */
    static void Submit(FBBBRifleRuntimeData &Data, const FBBBRifleReloadPacket &Packet);

    /** @param Data	步枪运行时数据 @param Packet	待消费请求 @return 无 */
    static void Submit(FBBBRifleRuntimeData &Data, const FBBBRifleDetachMagazinePacket &Packet);

    /** @param Data	步枪运行时数据 @param Packet	待消费请求 @return 无 */
    static void Submit(FBBBRifleRuntimeData &Data, const FBBBRifleLoadMagazinePacket &Packet);

    /** @param Data	步枪运行时数据 @param Packet	待消费请求 @return 无 */
    static void Submit(FBBBRifleRuntimeData &Data, const FBBBRifleInterruptReloadPacket &Packet);
};

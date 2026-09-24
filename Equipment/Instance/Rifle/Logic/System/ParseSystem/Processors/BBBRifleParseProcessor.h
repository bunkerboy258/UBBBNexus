#pragma once

#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/BBBRifleInputSubmit.h"
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

    /**
     * 按包的静态类型写入对应固定槽位
     * @param Data	步枪运行时数据
     * @param Packet	待提交请求
     * @return 无
     */
    template<typename TPacket>
    static void Submit(FBBBRifleRuntimeData &Data, const TPacket &Packet)
    {
        BBBRifleInput::Submit(Data.Parse.InputState, Packet);
    }
};

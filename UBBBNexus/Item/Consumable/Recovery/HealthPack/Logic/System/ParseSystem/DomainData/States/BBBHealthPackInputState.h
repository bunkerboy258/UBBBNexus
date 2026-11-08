#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Input/LocalControl/Use/FBBBHealthPackPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Input/LocalControl/Use/FBBBHealthPackFinishLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Recovery/HealthPack/Logic/System/ParseSystem/DomainData/Definitions/BBBHealthPackInputSlot.h"
/** 输入请求状态 */
struct FBBBHealthPackInputState final
{
    /** 动作阶段请求 */
    TBBBHealthPackInputSlot<FBBBHealthPackPrimaryLocalControlPacket> Primary;
    /** 使用结束请求 */
    TBBBHealthPackInputSlot<FBBBHealthPackFinishLocalControlPacket> Finish;

  private:
    friend struct FBBBHealthPackParseDomainState;
    FBBBHealthPackInputState() = default;
};

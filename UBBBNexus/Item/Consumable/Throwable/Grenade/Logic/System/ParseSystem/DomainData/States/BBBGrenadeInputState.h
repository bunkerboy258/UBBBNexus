#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Input/LocalControl/Use/FBBBGrenadePrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Input/LocalControl/Use/FBBBGrenadeFinishLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Throwable/Grenade/Logic/System/ParseSystem/DomainData/Definitions/BBBGrenadeInputSlot.h"
/** 输入请求状态 */
struct FBBBGrenadeInputState final
{
    /** 动作阶段请求 */
    TBBBGrenadeInputSlot<FBBBGrenadePrimaryLocalControlPacket> Primary;
    /** 使用结束请求 */
    TBBBGrenadeInputSlot<FBBBGrenadeFinishLocalControlPacket> Finish;

  private:
    friend struct FBBBGrenadeParseDomainState;
    FBBBGrenadeInputState() = default;
};

#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Logic/System/ParseSystem/Processors/BBBThrowableParseProcessor.h"
/** 输入系统固定解析入口 */
class FBBBThrowableParseSystem final
{
public:
    /** @param Actor	当前投掷物 @return 无 */
    void Update(ABBBThrowable& Actor) const { FBBBThrowableParseProcessor{}.Update(Actor); }
    /** @param Data	唯一黑板 @param Packet	独立输入 @return 是否接受 */
    template<typename TPacket>
    void Submit(FBBBThrowableRuntimeData& Data, TPacket&& Packet) const
    {
        FBBBThrowableParseProcessor::Submit(Data, Forward<TPacket>(Packet));
    }
};

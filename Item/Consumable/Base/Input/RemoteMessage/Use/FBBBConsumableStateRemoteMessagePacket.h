#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Network/BBBConsumableNetworkMessage.h"

/** 当前消耗品使用结果输入 */
struct FBBBConsumableStateRemoteMessagePacket final
{
    /** 同帧接收的结果 保留最新有效顺序 */
    TArray<FBBBConsumableNetworkMessage> Facts;

    /** @return 全部结果的结构是否有效 */
    bool IsValid() const
    {
        if (Facts.IsEmpty())
        {
            return false;
        }
        for (const auto &Fact : Facts)
        {
            if (!Fact.IsValid())
            {
                return false;
            }
        }
        return true;
    }

    /** @param Context	解析上下文 @return 是否只执行镜像还原 */
    template<typename TContext>
    bool CanApply(const TContext &Context) const
    {
        return Context.bIsMirror;
    }

    /** @param Context	解析上下文 @return 无 */
    template<typename TContext>
    void Apply(TContext &Context) const
    {
        for (const auto &Fact : Facts)
        {
            if (Fact.Revision <= Context.State.ReceivedRevision)
            {
                continue;
            }
            Context.State.ReceivedRevision = Fact.Revision;
            Context.State.UseSerial = Fact.UseSerial;
            Context.State.ReleaseSerial = Fact.ReleaseSerial;
            Context.State.bUsing = Fact.bUsing;
            Context.State.bReleased = Fact.bReleased;
            Context.State.LaunchTransform = Fact.LaunchTransform;
        }
    }
};

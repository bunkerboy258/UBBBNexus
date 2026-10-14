#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseInputState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/UseSystem/DomainData/States/BBBGrenadeUseState.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Base/Input/RemoteMessage/Use/FBBBConsumableStateRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Network/BBBGrenadeNetworkMessage.h"

/** 当前消耗品使用结果输入 */
struct FBBBGrenadeStateRemoteMessagePacket final : public FBBBConsumableStateRemoteMessagePacket<FBBBGrenadeStateRemoteMessagePacket>
{
    /** 同帧接收的结果 保留最新有效顺序 */
    TArray<FBBBGrenadeNetworkMessage> Facts;

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
    bool CanApply(bool bIsMirror) const
    {
        return bIsMirror;
    }

    /** @param Context	解析上下文 @return 无 */
    void Apply(FBBBGrenadeUseInputState &Inputs, FBBBGrenadeUseState &State) const
    {
        for (const auto &Fact : Facts)
        {
            if (Fact.Revision <= State.ReceivedRevision)
            {
                continue;
            }
            State.ReceivedRevision = Fact.Revision;
            State.UseSerial = Fact.UseSerial;
            State.ReleaseSerial = Fact.ReleaseSerial;
            State.bUsing = Fact.bUsing;
            State.bReleased = Fact.bReleased;
            State.LaunchTransform = Fact.LaunchTransform;
            State.InheritedVelocity = Fact.InheritedVelocity;
        }
    }
};

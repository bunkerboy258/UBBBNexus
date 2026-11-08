#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBCharacterPickupInboxState.h"
/** 角色连接收到的世界拾取待投送输入 */
struct FBBBCharacterPickupRequestRemoteMessagePacket final
{
    /** 本次待投送数据 */
    TArray<FBBBCharacterPickupRequest> Requests;
    /** @return 是否存在待投送数据 */
    bool IsValid() const
    {
        return !Requests.IsEmpty();
    }
    /** @param Context\t角色输入上下文 @return 是否可进入投送队列 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return true;
    }
    /** @param Context\t角色输入上下文 @return 无 只贡献投送数据 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.PickupInbox.Requests.Append(Requests);
    }
};

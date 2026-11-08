#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/NetworkSystem/DomainData/States/BBBCharacterPickupInboxState.h"
/** 角色连接收到的世界拾取待投送输入 */
struct FBBBCharacterPickupReceiptRemoteMessagePacket final
{
    /** 本次待投送数据 */
    TArray<FBBBCharacterPickupReceipt> Receipts;
    /** @return 是否存在待投送数据 */
    bool IsValid() const
    {
        return !Receipts.IsEmpty();
    }
    /** @param Context\t角色输入上下文 @return 是否可进入投送队列 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return true;
    }
    /** @param Context\t角色输入上下文 @return 无 只贡献投送数据 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.PickupInbox.Receipts.Append(Receipts);
    }
};

#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/DomainData/States/BBBCharacterPickupState.h"
/** 角色控制端接收待执行的物品交付 */
struct FBBBCharacterPickupGrantLocalControlPacket final
{
    /** 本次接收的全部交付 */
    TArray<FBBBCharacterPickupGrant> Grants;
    /** @return 交付结构是否完整 */
    bool IsValid() const
    {
        for (const auto &Grant : Grants)
        {
            if (!Grant.Drop || !Grant.DropId.IsValid() || Grant.Operation == 0 || Grant.ItemId.IsNone()
                || Grant.Quantity <= 0 || Grant.Quantity > 1000000)
            {
                return false;
            }
        }
        return !Grants.IsEmpty();
    }
    /** @param Context\t输入上下文 @return 是否维护真实物品 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror;
    }
    /** @param Context\t输入上下文 @return 无 只贡献交付 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.Pickup.PendingGrants.Append(Grants);
    }
};

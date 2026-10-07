#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentAnimationInputState.h"
/** 角色动画向当前装备贡献BeginAction通知 */
struct FBBBCharacterEquipmentBeginActionLocalControlPacket final
{
    /** 通知开始时绑定的实际装备 */
    TArray<TWeakObjectPtr<ABBBEquipment>> Recipients;
    /** 与每个装备配对的动画通知实例标识 */
    TArray<int32> Tokens;
    /** @return 配对的数据是否合法 */
    bool IsValid() const
    {
        return !Recipients.IsEmpty() && Recipients.Num() == Tokens.Num()
            && !Tokens.ContainsByPredicate([](int32 Value)
            {
                return Value < 0;
            });
    }
    /** @param Context 角色输入上下文 @return 是否可写入待转交通知 */
    bool CanApply(const FBBBCharacterInputContext &) const
    {
        return IsValid();
    }
    /** @param Context 角色输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.EquipmentAnimationInputs.BeginActionRecipients.Append(Recipients);
        Context.EquipmentAnimationInputs.BeginActionTokens.Append(Tokens);
    }
};

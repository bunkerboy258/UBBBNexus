#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/InteractionSystem/DomainData/States/BBBCharacterInteractionInputState.h"
/** 人物动画提交隐藏当前装备的请求 */
struct FBBBCharacterEquipmentHideLocalControlPacket final
{
    /** 通知触发时的目标人物持有代次 */
    TArray<uint64> Generations;
    /** @return 请求是否携带有效代次 */
    bool IsValid() const
    {
        return !Generations.IsEmpty();
    }
    /** @param Context	角色输入上下文 @return 请求是否允许转交 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return IsValid();
    }
    /** @param Context	角色输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.InteractionInputs.HideEquipmentGenerations.Append(Generations);
    }
};

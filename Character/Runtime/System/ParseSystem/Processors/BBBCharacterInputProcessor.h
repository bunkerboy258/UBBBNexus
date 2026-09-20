#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/State/BBBCharacterInputState.h"

class UBBBEquipmentCatalog;
struct FBBBCharacterInputContext;
struct FBBBCharacterRuntimeData;

/**
 * 按源码中明确声明的顺序原地解析固定输入槽位
 *
 * 处理器不理解具体业务规则，数据合法性、通过条件和状态效果全部由对应包负责
 */
class FBBBCharacterInputProcessor final
{
public:
    /**
     * 解析本次更新可见的固定输入槽位并发布角色控制
     * @param Data                  角色黑板
     * @param Catalog               装备目录
     * @param bAuthority            当前实例是否拥有玩法权威
     * @param bLocallyControlled    当前实例是否由本机控制
     */
    void Update(
        FBBBCharacterRuntimeData &Data,
        UBBBEquipmentCatalog &Catalog) const;

private:
    /**
     * 处理一个固定槽位并在处理后失活
     * @param Slot      待处理槽位
     * @param Context   角色输入上下文
     */
    template<typename TPacket>
    static void Process(
        TBBBCharacterInputSlot<TPacket> &Slot,
        FBBBCharacterInputContext &Context)
    {
        if (!Slot.bActive)
        {
            return;
        }

        if (Slot.Data.CanApply(Context))
        {
            Slot.Data.Apply(Context);
        }

        Slot.Consume();
    }

    /**
     * 发布数据包应用后的最终控制状态
     * @param Data 角色黑板
     */
    static void FinalizeControl(FBBBCharacterRuntimeData &Data);
};

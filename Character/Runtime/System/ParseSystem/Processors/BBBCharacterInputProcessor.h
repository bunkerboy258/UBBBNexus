#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInputFrame.h"

class UBBBEquipmentCatalog;
struct FBBBCharacterPacketContext;
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
        UBBBEquipmentCatalog &Catalog,
        bool bAuthority,
        bool bLocallyControlled) const;

private:
    /**
     * 处理一个固定槽位并在处理后失活
     * @param Slot      待处理槽位
     * @param Context   角色输入上下文
     */
    template<typename TPacket>
    static void Process(
        TBBBCharacterInputSlot<TPacket> &Slot,
        FBBBCharacterPacketContext &Context)
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
     * 处理需要由本机客户端继续上传给权威端的槽位
     *
     * 本机非权威实例保留激活标记，网络命令处理器发送后负责失活；其他实例正常消费
     *
     * @param Slot                  待处理槽位
     * @param Context               角色输入上下文
     * @param bPreserveForNetwork   是否保留给网络命令处理器
     */
    template<typename TPacket>
    static void ProcessNetworkCommand(
        TBBBCharacterInputSlot<TPacket> &Slot,
        FBBBCharacterPacketContext &Context,
        const bool bPreserveForNetwork)
    {
        if (!Slot.bActive)
        {
            return;
        }

        if (Slot.Data.CanApply(Context))
        {
            Slot.Data.Apply(Context);
        }

        if (!bPreserveForNetwork)
        {
            Slot.Consume();
        }
    }

    /**
     * 发布数据包应用后的最终控制状态
     * @param Data 角色黑板
     */
    static void FinalizeControl(FBBBCharacterRuntimeData &Data);
};

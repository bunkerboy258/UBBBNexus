#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

/**
 * 快捷槽切枪请求包 请求带内最高优先级
 * 执行时取消进行中的换弹并改写期望主手装备
 */
struct FBBBEquipSlotPacket
{
    static constexpr int32 Priority = 19;

    static constexpr uint64 ApprovedBit = 1ull << 9;

    /** 目标快捷槽位 */
    int32 Slot = INDEX_NONE;

    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return Slot != INDEX_NONE;
    }

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanExecute(const FBBBCharacterPacketContext &Context) const
    {
        // 还原模式不接受本地请求 空槽 无效槽与当前装备同槽都不构成切换
        if (Context.Operation.IsRestoreMode()
            || !Context.Inventory.QuickAccessBindings.IsValidIndex(Slot))
        {
            return false;
        }

        ABBBEquipment *Target = Context.Inventory.QuickAccessBindings[Slot];
        return ::IsValid(Target) && Target != Context.Equipment.GetActiveMainHandInstance();
    }

    /**
     * 取消换弹并登记期望主手装备
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const
    {
        // 切换装备打断进行中的换弹
        if (Context.Operation.IsReloadInProgress())
        {
            Context.Operation.CancelReload();
        }

        ABBBEquipment *Target = Context.Inventory.QuickAccessBindings[Slot];
        Context.Operation.SelectEquipment(*Target);
        Context.Equipment.SetDesiredMainHandInstance(Target);
    }
};

#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipmentActionFact.h"

/**
 * 开火已执行事实包 本地由装备回执 还原模式由网络还原
 * 还原时先中断本地追踪的换弹
 */
struct FBBBFireFactPacket
{
    static constexpr int32 Priority = 50;

    static constexpr uint64 ApprovedBit = 1ull << 4;

    /** 线上身份 与网络包 PacketId 对应 */
    static constexpr uint8 PacketId = 2;

    FName EquipmentId = NAME_None;

    int32 Sequence = INDEX_NONE;

    int32 LoadedAmmo = 0;

    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return Sequence > 0;
    }

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanExecute(const FBBBCharacterPacketContext &Context) const
    {
        return true;
    }

    /**
     * 留档事实 还原模式先取消换弹再下发镜像装备快照
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const
    {
        if (Context.Operation.IsRestoreMode())
        {
            // 远端开火意味着其换弹不再有效 本地追踪同步中止
            if (Context.Operation.IsReloadInProgress())
            {
                Context.Operation.CancelReload();
            }

            Context.Commands.SubmitRestoredAction(ToFact());
        }

        Context.Events.AddAction(ToFact());
    }

    /** @return 对应的事实记录 */
    FBBBEquipmentActionFact ToFact() const
    {
        return FBBBEquipmentActionFact{PacketId, EquipmentId, Sequence, LoadedAmmo};
    }
};

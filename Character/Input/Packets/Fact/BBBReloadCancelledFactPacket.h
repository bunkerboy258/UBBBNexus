#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipmentActionFact.h"

/**
 * 换弹已取消事实包 驱动解析状态机完结换弹追踪并记录取消序号
 */
struct FBBBReloadCancelledFactPacket
{
    static constexpr int32 Priority = 50;

    static constexpr uint64 ApprovedBit = 1ull << 8;

    /** 线上身份 与网络包 PacketId 对应 */
    static constexpr uint8 PacketId = 6;

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
     * 留档事实并以取消方式完结换弹追踪 还原模式同时下发镜像装备快照
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const
    {
        if (Context.Operation.IsRestoreMode())
        {
            Context.Commands.SubmitRestoredAction(ToFact());
        }

        Context.Events.AddAction(ToFact());
        Context.Operation.TrackReloadFinished(Sequence, EquipmentId, Context.Equipment.GetActiveEquipmentId(), true);
    }

    /** @return 对应的事实记录 */
    FBBBEquipmentActionFact ToFact() const
    {
        return FBBBEquipmentActionFact{PacketId, EquipmentId, Sequence, LoadedAmmo};
    }
};

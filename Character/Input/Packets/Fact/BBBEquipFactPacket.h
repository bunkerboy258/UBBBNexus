#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipmentActionFact.h"

/**
 * 装备切换已执行事实包 本地由装备回执 还原模式由网络还原
 */
struct FBBBEquipFactPacket
{
    static constexpr int32 Priority = 50;

    static constexpr uint64 ApprovedBit = 1ull << 3;

    /** 线上身份 与网络包 PacketId 对应 */
    static constexpr uint8 PacketId = 1;

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
     * 留档事实 还原模式同时下发镜像装备快照
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const
    {
        // 还原模式下事实要交给镜像装备还原 本地模式仅留档供网络上传
        if (Context.Operation.IsRestoreMode())
        {
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

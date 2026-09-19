#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

struct FBBBEquipmentActionFact;

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
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanExecute(const FBBBCharacterPacketContext &Context) const;

    /**
     * 留档事实 还原模式同时下发镜像装备快照
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const;

    /** @return 对应的事实记录 */
    FBBBEquipmentActionFact ToFact() const;
};

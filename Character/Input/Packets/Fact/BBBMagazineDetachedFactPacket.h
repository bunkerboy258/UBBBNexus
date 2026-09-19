#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

struct FBBBEquipmentActionFact;

/**
 * 弹匣已卸下事实包 驱动解析状态机记录卸下进度
 */
struct FBBBMagazineDetachedFactPacket
{
    static constexpr int32 Priority = 50;

    static constexpr uint64 ApprovedBit = 1ull << 6;

    /** 线上身份 与网络包 PacketId 对应 */
    static constexpr uint8 PacketId = 4;

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
     * 留档事实并记录卸下进度 还原模式同时下发镜像装备快照
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const;

    /** @return 对应的事实记录 */
    FBBBEquipmentActionFact ToFact() const;
};

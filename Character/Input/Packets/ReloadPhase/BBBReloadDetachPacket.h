#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

/**
 * 换弹卸下弹匣动画通知包
 * 序号必须匹配当前换弹且尚未卸下
 */
struct FBBBReloadDetachPacket
{
    static constexpr int32 Priority = 15;

    static constexpr uint64 ApprovedBit = 1ull << 13;

    /** 播放时保存的换弹序号 */
    int32 Sequence = INDEX_NONE;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanExecute(const FBBBCharacterPacketContext &Context) const;

    /**
     * 推进卸下阶段并转发装备命令
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const;
};

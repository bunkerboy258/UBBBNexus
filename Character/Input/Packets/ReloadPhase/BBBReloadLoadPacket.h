#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

/**
 * 换弹装填弹匣动画通知包
 * 装填必须先于卸下之后到达
 */
struct FBBBReloadLoadPacket
{
    static constexpr int32 Priority = 15;

    static constexpr uint64 ApprovedBit = 1ull << 14;

    /** 播放时保存的换弹序号 */
    int32 Sequence = INDEX_NONE;

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
        // 还原模式不由本地动画通知驱动换弹阶段
        return !Context.Operation.IsRestoreMode();
    }

    /**
     * 推进装填阶段并转发装备命令
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const
    {
        // 序号守卫与先卸后装守卫由解析状态机集中裁决
        if (Context.Operation.ReportMagazineLoaded(Sequence))
        {
            Context.Commands.SubmitLoadMagazine(Sequence);
        }
    }
};

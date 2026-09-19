#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

/**
 * 换弹中断动画通知包
 * 未收到装填通知的播放结束由此收尾
 */
struct FBBBReloadInterruptPacket
{
    static constexpr int32 Priority = 15;

    static constexpr uint64 ApprovedBit = 1ull << 15;

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
     * 登记换弹中断并转发装备取消命令
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const
    {
        // 序号守卫由解析状态机集中裁决
        if (Context.Operation.ReportReloadInterrupted(Sequence))
        {
            Context.Commands.SubmitCancelReload(Sequence);
        }
    }
};

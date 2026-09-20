#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Context/BBBCharacterInputContext.h"

/**
 * 换弹中断动画通知包
 * 未收到装填通知的播放结束由此收尾
 */
struct FBBBReloadInterruptPacket final
{

    /** 播放时保存的换弹序号 */
    int32 Sequence = INDEX_NONE;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanApply(const FBBBCharacterInputContext &Context) const;

    /**
     * 登记换弹中断并转发装备取消命令
     * @param Context	黑板上下文
     */
    void Apply(FBBBCharacterInputContext &Context) const;
};

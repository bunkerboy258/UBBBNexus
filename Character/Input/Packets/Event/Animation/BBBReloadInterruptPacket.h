#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/System/ParseSystem/Context/BBBCharacterInputContext.h"

/**
 * 换弹中断动画通知包
 */
struct FBBBReloadInterruptPacket final
{

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

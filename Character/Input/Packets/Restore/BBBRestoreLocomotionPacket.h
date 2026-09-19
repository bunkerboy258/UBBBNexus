#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

/**
 * 网络还原步态包 直接提交步态黑板
 */
struct FBBBRestoreLocomotionPacket final
{

    /** 远端同步的移动步态 */
    EBBBCharacterGait Gait = EBBBCharacterGait::Run;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanApply(const FBBBCharacterPacketContext &Context) const;

    /**
     * 还原移动步态
     * @param Context	黑板上下文
     */
    void Apply(FBBBCharacterPacketContext &Context) const;
};

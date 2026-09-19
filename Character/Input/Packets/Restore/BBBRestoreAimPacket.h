#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

/**
 * 网络还原瞄准状态包 直接覆盖瞄准黑板
 */
struct FBBBRestoreAimPacket
{
    static constexpr int32 Priority = 100;

    static constexpr uint64 ApprovedBit = 1ull << 1;

    /** 远端同步的瞄准状态 */
    FBBBAimRuntimeState State;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanExecute(const FBBBCharacterPacketContext &Context) const;

    /**
     * 还原瞄准黑板状态
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const;
};

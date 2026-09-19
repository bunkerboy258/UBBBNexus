#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

/**
 * 跳跃请求包 与装备动作无冲突
 */
struct FBBBJumpPacket
{
    static constexpr int32 Priority = 16;

    static constexpr uint64 ApprovedBit = 1ull << 12;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanExecute(const FBBBCharacterPacketContext &Context) const;

    /**
     * 登记跳跃意图到控制事实
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const;
};

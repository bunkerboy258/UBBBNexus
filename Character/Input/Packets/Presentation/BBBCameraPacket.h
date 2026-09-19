#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

/**
 * 相机表现贡献包 提交一次冲量与恢复速度
 */
struct FBBBCameraPacket
{
    static constexpr int32 Priority = 4;

    static constexpr uint64 ApprovedBit = 1ull << 17;

    FVector2D Impulse = FVector2D::ZeroVector;

    float RecoverySpeed = 0.0f;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanExecute(const FBBBCharacterPacketContext &Context) const;

    /**
     * 追加相机表现贡献
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const;
};

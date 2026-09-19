#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

class UAnimMontage;

/**
 * 蒙太奇表现包 进入五槽位期望体系等待动画系统按修订号播放
 * 包存活仅一帧 蒙太奇资产由装备配置持有 不参与反射
 */
struct FBBBMontagePacket
{
    static constexpr int32 Priority = 5;

    static constexpr uint64 ApprovedBit = 1ull << 16;

    TObjectPtr<UAnimMontage> Montage = nullptr;

    float PlayRate = 1.0f;

    int32 Sequence = INDEX_NONE;

    /** 是否换弹蒙太奇 换弹蒙太奇受序号守卫 */
    bool bReload = false;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanExecute(const FBBBCharacterPacketContext &Context) const;

    /**
     * 通过槽位守卫后写入槽位期望
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const;
};

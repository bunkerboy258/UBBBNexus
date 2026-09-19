#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

/**
 * 快捷槽切枪请求包 请求带内最高优先级
 * 执行时取消进行中的换弹并改写期望主手装备
 */
struct FBBBEquipSlotPacket final
{

    /** 目标快捷槽位 */
    int32 Slot = INDEX_NONE;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanApply(const FBBBCharacterPacketContext &Context) const;

    /**
     * 取消换弹并登记期望主手装备
     * @param Context	黑板上下文
     */
    void Apply(FBBBCharacterPacketContext &Context) const;
};

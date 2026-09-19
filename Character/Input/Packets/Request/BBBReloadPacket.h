#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"

/**
 * 换弹请求包 请求带内输给切枪
 * 批准时压过同帧开火
 */
struct FBBBReloadPacket final
{

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanApply(const FBBBCharacterPacketContext &Context) const;

    /**
     * 登记换弹承诺并提交装备换弹命令
     * @param Context	黑板上下文
     */
    void Apply(FBBBCharacterPacketContext &Context) const;
};

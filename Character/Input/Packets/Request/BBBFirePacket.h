#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBEquipSlotPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBReloadPacket.h"

/**
 * 开火请求包 请求带内输给切枪与换弹
 */
struct FBBBFirePacket
{
    static constexpr int32 Priority = 17;

    static constexpr uint64 ApprovedBit = 1ull << 11;

    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return true;
    }

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanExecute(const FBBBCharacterPacketContext &Context) const
    {
        // 还原模式不接受本地请求 同帧切枪或换弹已批准时让步
        return !Context.Operation.IsRestoreMode()
            && !Context.Approved.HasAny<FBBBEquipSlotPacket, FBBBReloadPacket>();
    }

    /**
     * 登记开火事实并提交装备开火命令
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const
    {
        Context.Operation.CommitFire();
        Context.Commands.SubmitFire();
    }
};

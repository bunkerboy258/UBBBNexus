#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBEquipSlotPacket.h"

/**
 * 换弹请求包 请求带内输给切枪
 * 批准时压过同帧开火
 */
struct FBBBReloadPacket
{
    static constexpr int32 Priority = 18;

    static constexpr uint64 ApprovedBit = 1ull << 10;

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
        // 还原模式不接受本地请求 空手 换弹进行中与同帧切枪都拒绝换弹
        return !Context.Operation.IsRestoreMode()
            && Context.Equipment.GetActiveMainHandInstance() != nullptr
            && !Context.Operation.IsReloadInProgress()
            && !Context.Approved.HasAny<FBBBEquipSlotPacket>();
    }

    /**
     * 登记换弹承诺并提交装备换弹命令
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const
    {
        Context.Operation.CommitReload();
        Context.Commands.SubmitReload();
    }
};

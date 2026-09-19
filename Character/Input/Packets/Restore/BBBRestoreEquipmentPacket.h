#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/BBBCharacterPacketContext.h"
#include "BBBWork/UBBBNexus/Equipment/Catalog/BBBEquipmentCatalog.h"

/**
 * 网络还原装备选择包 将装备句柄还原为镜像装备创建请求
 */
struct FBBBRestoreEquipmentPacket
{
    static constexpr int32 Priority = 100;

    static constexpr uint64 ApprovedBit = 1ull << 0;

    /** 远端主手装备的句柄 对应装备配置的 EquipmentId */
    FName EquipmentHandle = NAME_None;

    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return EquipmentHandle != NAME_None;
    }

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanExecute(const FBBBCharacterPacketContext &Context) const
    {
        // 还原包只在还原模式生效 本地角色的装备选择由请求带决定
        return Context.Operation.IsRestoreMode();
    }

    /**
     * 按句柄查找装备配置并登记镜像创建请求
     * @param Context	黑板上下文
     */
    void Execute(FBBBCharacterPacketContext &Context) const
    {
        UBBBEquipmentDefinition *Definition = Context.Catalog.FindDefinition(EquipmentHandle);
        if (ensureMsgf(Definition, TEXT("[UBBBC]Unknown equipment restore handle %s"), *EquipmentHandle.ToString()))
        {
            Context.Commands.SubmitRestoredEquipment(*Definition);
        }
    }
};

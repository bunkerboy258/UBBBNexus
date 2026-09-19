#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterPacketContext.h"

/**
 * 网络还原装备选择包 将装备句柄还原为镜像装备创建请求
 */
struct FBBBRestoreEquipmentPacket final
{

    /** 远端主手装备的句柄 对应装备配置的 EquipmentId */
    FName EquipmentHandle = NAME_None;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanApply(const FBBBCharacterPacketContext &Context) const;

    /**
     * 按句柄查找装备配置并登记镜像创建请求
     * @param Context	黑板上下文
     */
    void Apply(FBBBCharacterPacketContext &Context) const;
};

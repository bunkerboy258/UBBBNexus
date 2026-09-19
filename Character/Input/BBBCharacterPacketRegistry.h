#pragma once

#include "CoreMinimal.h"
#include "Misc/TVariant.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Base/BBBCharacterAimPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Base/BBBCharacterMovementPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBEquipFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBFireFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBMagazineDetachedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Fact/BBBReloadStartedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBCameraPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/ReloadPhase/BBBReloadDetachPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/ReloadPhase/BBBReloadInterruptPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/ReloadPhase/BBBReloadLoadPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBEquipSlotPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBFirePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBJumpPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Request/BBBReloadPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Restore/BBBRestoreAimPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Restore/BBBRestoreEquipmentPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Restore/BBBRestoreLocomotionPacket.h"

// ======================= 帧仲裁全景 =======================
// 所有输入统一为离散快照 每帧压入黑板快照区队列 排序后一次消费
// 还原带 100 RestoreEquipment RestoreAim RestoreLocomotion 保到达序
// 事实带  50 Equip Fire ReloadStarted MagazineDetached MagazineLoaded ReloadCancelled 保到达序
// 基底带  30 Movement Aim 每帧覆盖控制基座 本帧缺席时黑板保留上帧值
// 请求带  EquipSlot(19) 否决 Reload(18) 否决 Fire(17)  Jump(16) 独立
// 阶段带  15 ReloadDetach ReloadLoad ReloadInterrupt 序号守卫
// 表现带  Montage(5) Camera(4) 恒可执行
// 请求带执行两阶段 先按序求值 CanExecute 再统一提交 Execute
// 请求带铁律 CanExecute 只读解析状态与已批准集合 禁止读控制基座
// 其余带惰性求值 轮到即判即行 保持帧内因果链
// 派生门控 瞄准或开火禁冲刺 由仲裁器 Finalize 统一处理
// ==========================================================

/** 角色离散快照包的封闭类型集合 新增包在此登记 */
using FBBBCharacterPacket = TVariant<
    FBBBRestoreEquipmentPacket,
    FBBBRestoreAimPacket,
    FBBBRestoreLocomotionPacket,
    FBBBEquipFactPacket,
    FBBBFireFactPacket,
    FBBBReloadStartedFactPacket,
    FBBBMagazineDetachedFactPacket,
    FBBBMagazineLoadedFactPacket,
    FBBBReloadCancelledFactPacket,
    FBBBCharacterMovementPacket,
    FBBBCharacterAimPacket,
    FBBBEquipSlotPacket,
    FBBBReloadPacket,
    FBBBFirePacket,
    FBBBJumpPacket,
    FBBBReloadDetachPacket,
    FBBBReloadLoadPacket,
    FBBBReloadInterruptPacket,
    FBBBMontagePacket,
    FBBBCameraPacket>;

/** 输入包优先级带边界 */
namespace BBBCharacterPacketPriority
{
    inline constexpr int32 Restore = 100;
    inline constexpr int32 Fact = 50;
    inline constexpr int32 Base = 30;
    inline constexpr int32 RequestMax = 19;
    inline constexpr int32 RequestMin = 16;
    inline constexpr int32 ReloadPhase = 15;
    inline constexpr int32 PresentationMax = 5;
}

namespace BBBCharacterPacketRegistryDetail
{
    /** @return 请求带优先级是否互不相同 */
    consteval bool AreRequestPrioritiesUnique()
    {
        constexpr int32 Priorities[] = {
            FBBBEquipSlotPacket::Priority,
            FBBBReloadPacket::Priority,
            FBBBFirePacket::Priority,
            FBBBJumpPacket::Priority};

        for (int32 First = 0; First < 4; ++First)
        {
            for (int32 Second = First + 1; Second < 4; ++Second)
            {
                if (Priorities[First] == Priorities[Second])
                {
                    return false;
                }
            }
        }

        return true;
    }

    /** @return 全部包的身份位是否互不相同 */
    consteval bool AreApprovedBitsUnique()
    {
        constexpr uint64 Bits[] = {
            FBBBRestoreEquipmentPacket::ApprovedBit,
            FBBBRestoreAimPacket::ApprovedBit,
            FBBBRestoreLocomotionPacket::ApprovedBit,
            FBBBEquipFactPacket::ApprovedBit,
            FBBBFireFactPacket::ApprovedBit,
            FBBBReloadStartedFactPacket::ApprovedBit,
            FBBBMagazineDetachedFactPacket::ApprovedBit,
            FBBBMagazineLoadedFactPacket::ApprovedBit,
            FBBBReloadCancelledFactPacket::ApprovedBit,
            FBBBCharacterMovementPacket::ApprovedBit,
            FBBBCharacterAimPacket::ApprovedBit,
            FBBBEquipSlotPacket::ApprovedBit,
            FBBBReloadPacket::ApprovedBit,
            FBBBFirePacket::ApprovedBit,
            FBBBJumpPacket::ApprovedBit,
            FBBBReloadDetachPacket::ApprovedBit,
            FBBBReloadLoadPacket::ApprovedBit,
            FBBBReloadInterruptPacket::ApprovedBit,
            FBBBMontagePacket::ApprovedBit,
            FBBBCameraPacket::ApprovedBit};

        for (int32 First = 0; First < 20; ++First)
        {
            for (int32 Second = First + 1; Second < 20; ++Second)
            {
                if (Bits[First] == Bits[Second])
                {
                    return false;
                }
            }
        }

        return true;
    }
}

// 编译期防呆 请求带优先级撞车会让仲裁结果不确定 身份位撞车会让冲突判断失效
static_assert(BBBCharacterPacketRegistryDetail::AreRequestPrioritiesUnique());
static_assert(BBBCharacterPacketRegistryDetail::AreApprovedBitsUnique());
static_assert(FBBBRestoreEquipmentPacket::Priority == BBBCharacterPacketPriority::Restore);
static_assert(FBBBEquipFactPacket::Priority == BBBCharacterPacketPriority::Fact);
static_assert(FBBBCharacterMovementPacket::Priority == BBBCharacterPacketPriority::Base);
static_assert(FBBBCharacterAimPacket::Priority == BBBCharacterPacketPriority::Base);

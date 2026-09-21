#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/BBBCharacterInputSlot.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBEquipSlotPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBFirePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBJumpPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Command/BBBReloadPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBEquipFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBFireFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineDetachedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBMagazineLoadedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadCancelledFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Event/Equipment/BBBReloadStartedFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBCameraPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/Presentation/BBBMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBAimStatePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBCharacterAimPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBCharacterMovementPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBEquipmentStatePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Packets/State/BBBLocomotionStatePacket.h"

/** 角色全部输入的固定槽位状态 */
struct FBBBCharacterInputState final
{
    /** 装备状态输入槽位 */
    TBBBCharacterInputSlot<FBBBEquipmentStatePacket> EquipmentState;
    /** 瞄准状态输入槽位 */
    TBBBCharacterInputSlot<FBBBAimStatePacket> AimState;
    /** 移动状态输入槽位 */
    TBBBCharacterInputSlot<FBBBLocomotionStatePacket> LocomotionState;
    /** 装备完成事实输入槽位 */
    TBBBCharacterInputSlot<FBBBEquipFactPacket> EquipFact;
    /** 开火事实输入槽位 */
    TBBBCharacterInputSlot<FBBBFireFactPacket> FireFact;
    /** 换弹开始事实输入槽位 */
    TBBBCharacterInputSlot<FBBBReloadStartedFactPacket> ReloadStartedFact;
    /** 弹匣脱离事实输入槽位 */
    TBBBCharacterInputSlot<FBBBMagazineDetachedFactPacket> MagazineDetachedFact;
    /** 弹匣装入事实输入槽位 */
    TBBBCharacterInputSlot<FBBBMagazineLoadedFactPacket> MagazineLoadedFact;
    /** 换弹取消事实输入槽位 */
    TBBBCharacterInputSlot<FBBBReloadCancelledFactPacket> ReloadCancelledFact;
    /** 移动命令输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterMovementPacket> Movement;
    /** 瞄准命令输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterAimPacket> Aim;
    /** 装备栏切换命令输入槽位 */
    TBBBCharacterInputSlot<FBBBEquipSlotPacket> EquipSlot;
    /** 换弹命令输入槽位 */
    TBBBCharacterInputSlot<FBBBReloadPacket> Reload;
    /** 开火命令输入槽位 */
    TBBBCharacterInputSlot<FBBBFirePacket> Fire;
    /** 跳跃命令输入槽位 */
    TBBBCharacterInputSlot<FBBBJumpPacket> Jump;
    /** 全身蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBFullBodyMontagePacket> FullBodyMontage;
    /** 上半身蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBUpperBodyMontagePacket> UpperBodyMontage;
    /** 瞄准前全身叠加蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBFullBodyAdditivePreAimMontagePacket> FullBodyAdditivePreAimMontage;
    /** 上半身叠加蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBUpperBodyAdditiveMontagePacket> UpperBodyAdditiveMontage;
    /** 受击叠加蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBAdditiveHitReactMontagePacket> AdditiveHitReactMontage;
    /** 摄像机输入槽位 */
    TBBBCharacterInputSlot<FBBBCameraPacket> Camera;

    /** 输入处理器是否正在读取固定槽位 */
    bool bProcessing = false;
};

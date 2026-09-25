#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Definitions/BBBCharacterInputSlot.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Locomotion/FBBBJumpLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Locomotion/FBBBRunLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Locomotion/FBBBCrouchLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Camera/FBBBCameraLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBFullBodyMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBUpperBodyMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBFullBodyAdditivePreAimMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBUpperBodyAdditiveMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBAdditiveHitReactMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Aim/FBBBAimStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Aim/FBBBCharacterAimLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Locomotion/FBBBCharacterMovementLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Locomotion/FBBBRunStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Equipment/FBBBEquipmentSelectionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Equipment/FBBBEquipmentSelectionAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBFullBodyMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBUpperBodyMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBFullBodyAdditivePreAimMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBUpperBodyAdditiveMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBAdditiveHitReactMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Equipment/FBBBEquipmentSlotLocalControlPacket.h"

/** 角色全部输入的固定槽位状态 */
struct FBBBCharacterInputState final
{
    /** 瞄准状态输入槽位 */
    TBBBCharacterInputSlot<FBBBAimStateAuthorityFactPacket> AimState;
    /** 跑步状态输入槽位 */
    TBBBCharacterInputSlot<FBBBRunStateAuthorityFactPacket> RunState;
    /** 装备选择输入槽位 */
    TBBBCharacterInputSlot<FBBBEquipmentSelectionLocalControlPacket> EquipmentSelectionState;
    /** 权威装备选择事实输入槽位 */
    TBBBCharacterInputSlot<FBBBEquipmentSelectionAuthorityFactPacket> AuthorityEquipmentSelectionState;
    /** 快捷栏装备选择输入槽位 */
    TBBBCharacterInputSlot<FBBBEquipmentSlotLocalControlPacket> EquipmentSlot;
    /** 移动命令输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterMovementLocalControlPacket> Movement;
    /** 跑步动作输入槽位 */
    TBBBCharacterInputSlot<FBBBRunLocalControlPacket> Run;
    /** 蹲伏动作输入槽位 */
    TBBBCharacterInputSlot<FBBBCrouchLocalControlPacket> Crouch;
    /** 瞄准命令输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterAimLocalControlPacket> Aim;
    /** 跳跃命令输入槽位 */
    TBBBCharacterInputSlot<FBBBJumpLocalControlPacket> Jump;
    /** 全身蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBFullBodyMontageLocalControlPacket> FullBodyMontage;
    /** 权威全身蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBFullBodyMontageAuthorityFactPacket> AuthorityFullBodyMontage;
    /** 上半身蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBUpperBodyMontageLocalControlPacket> UpperBodyMontage;
    /** 权威上半身蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBUpperBodyMontageAuthorityFactPacket> AuthorityUpperBodyMontage;
    /** 瞄准前全身叠加蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBFullBodyAdditivePreAimMontageLocalControlPacket> FullBodyAdditivePreAimMontage;
    /** 权威瞄准前全身叠加蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBFullBodyAdditivePreAimMontageAuthorityFactPacket> AuthorityFullBodyAdditivePreAimMontage;
    /** 上半身叠加蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBUpperBodyAdditiveMontageLocalControlPacket> UpperBodyAdditiveMontage;
    /** 权威上半身叠加蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBUpperBodyAdditiveMontageAuthorityFactPacket> AuthorityUpperBodyAdditiveMontage;
    /** 受击叠加蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBAdditiveHitReactMontageLocalControlPacket> AdditiveHitReactMontage;
    /** 权威受击叠加蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBAdditiveHitReactMontageAuthorityFactPacket> AuthorityAdditiveHitReactMontage;
    /** 摄像机输入槽位 */
    TBBBCharacterInputSlot<FBBBCameraLocalControlPacket> Camera;
};

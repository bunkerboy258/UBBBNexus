#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Definitions/BBBCharacterInputSlot.h"
#include "BBBWork/UBBBNexus/Character/Input/Local/Action/BBBJumpPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Local/Action/BBBRunPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Local/Action/BBBCrouchPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Local/Camera/BBBCameraPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Shared/Animation/BBBFullBodyMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Shared/Animation/BBBUpperBodyMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Shared/Animation/BBBFullBodyAdditivePreAimMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Shared/Animation/BBBUpperBodyAdditiveMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Shared/Animation/BBBAdditiveHitReactMontagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Mirror/State/BBBAimStatePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Local/Control/BBBCharacterAimPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Local/Control/BBBCharacterMovementPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/Mirror/State/BBBRunStatePacket.h"

/** 角色全部输入的固定槽位状态 */
struct FBBBCharacterInputState final
{
    /** 瞄准状态输入槽位 */
    TBBBCharacterInputSlot<FBBBAimStatePacket> AimState;
    /** 跑步状态输入槽位 */
    TBBBCharacterInputSlot<FBBBRunStatePacket> RunState;
    /** 移动命令输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterMovementPacket> Movement;
    /** 跑步动作输入槽位 */
    TBBBCharacterInputSlot<FBBBRunPacket> Run;
    /** 蹲伏动作输入槽位 */
    TBBBCharacterInputSlot<FBBBCrouchPacket> Crouch;
    /** 瞄准命令输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterAimPacket> Aim;
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
};

#pragma once
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Equipment/FBBBEquipmentSelectionRemoteMessagePacket.h"

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Life/FBBBCharacterLifeAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterLifeRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterDamageDeliveryRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Equipment/FBBBCharacterEquipmentUseAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Equipment/FBBBCharacterEquipmentUseRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterDamageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBAimImpulseLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBAimImpulseAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Definitions/BBBCharacterInputSlot.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Locomotion/FBBBJumpLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Locomotion/FBBBRunLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Locomotion/FBBBCrouchLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Camera/FBBBCameraLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBFullBodyMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBTraversalMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBUpperBodyMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBFullBodyAdditivePreAimMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBUpperBodyAdditiveMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBAdditiveHitReactMontageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Aim/FBBBAimStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Aim/FBBBCharacterAimLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Locomotion/FBBBCharacterMovementLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Locomotion/FBBBRunStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Locomotion/FBBBAccelerationAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Locomotion/FBBBAccelerationRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBItemAddLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBItemMoveLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Equipment/FBBBEquipmentSelectionAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBFullBodyMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBUpperBodyMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBFullBodyAdditivePreAimMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBUpperBodyAdditiveMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBAdditiveHitReactMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBItemSelectLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Traversal/FBBBTraversalStartRemoteMessagePacket.h"

#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Traversal/FBBBTraversalEndRemoteMessagePacket.h"

#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Traversal/FBBBTraversalStartAuthorityFactPacket.h"

#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Traversal/FBBBTraversalEndAuthorityFactPacket.h"

#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Equipment/FBBBCharacterEquipmentBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Equipment/FBBBCharacterEquipmentEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Equipment/FBBBCharacterEquipmentBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Equipment/FBBBCharacterEquipmentEndContactLocalControlPacket.h"

/** 角色全部输入的固定槽位状态 */
struct FBBBCharacterInputState final
{
    /** 接收的生命结果 */
    TBBBCharacterInputSlot<FBBBCharacterLifeAuthorityFactPacket> AuthorityLife;
    /** 控制者生成的生命结果 */
    TBBBCharacterInputSlot<FBBBCharacterLifeRemoteMessagePacket> RemoteLife;
    /** 需向控制者投送的独立命中 */
    TBBBCharacterInputSlot<FBBBCharacterDamageDeliveryRemoteMessagePacket> DamageDelivery;
    /** 装备独立使用结果输入 */
    TBBBCharacterInputSlot<FBBBCharacterEquipmentUseAuthorityFactPacket> AuthorityFactEquipmentUse;
    /** 装备独立使用结果输入 */
    TBBBCharacterInputSlot<FBBBCharacterEquipmentUseRemoteMessagePacket> RemoteMessageEquipmentUse;
    /** 本帧全部独立伤害输入 */
    TBBBCharacterInputSlot<FBBBCharacterDamageLocalControlPacket> Damage;
    /** 装备BeginAction通知输入槽 */
    TBBBCharacterInputSlot<FBBBCharacterEquipmentBeginActionLocalControlPacket> EquipmentBeginAction;
    /** 装备EndAction通知输入槽 */
    TBBBCharacterInputSlot<FBBBCharacterEquipmentEndActionLocalControlPacket> EquipmentEndAction;
    /** 装备BeginContact通知输入槽 */
    TBBBCharacterInputSlot<FBBBCharacterEquipmentBeginContactLocalControlPacket> EquipmentBeginContact;
    /** 装备EndContact通知输入槽 */
    TBBBCharacterInputSlot<FBBBCharacterEquipmentEndContactLocalControlPacket> EquipmentEndContact;
    /** 翻越StartRemoteMessage输入槽位 */
    TBBBCharacterInputSlot<FBBBTraversalStartRemoteMessagePacket> TraversalStartRemoteMessage;
    /** 翻越EndRemoteMessage输入槽位 */
    TBBBCharacterInputSlot<FBBBTraversalEndRemoteMessagePacket> TraversalEndRemoteMessage;
    /** 翻越StartAuthorityFact输入槽位 */
    TBBBCharacterInputSlot<FBBBTraversalStartAuthorityFactPacket> TraversalStartAuthorityFact;
    /** 翻越EndAuthorityFact输入槽位 */
    TBBBCharacterInputSlot<FBBBTraversalEndAuthorityFactPacket> TraversalEndAuthorityFact;
    /** 瞄准状态输入槽位 */
    TBBBCharacterInputSlot<FBBBAimStateAuthorityFactPacket> AimState;
    /** 跑步状态输入槽位 */
    TBBBCharacterInputSlot<FBBBRunStateAuthorityFactPacket> RunState;
    /** 控制者已经产生的加速度结果 */
    TBBBCharacterInputSlot<FBBBAccelerationRemoteMessagePacket> RemoteAcceleration;
    /** 当前加速度事实的镜像还原 */
    TBBBCharacterInputSlot<FBBBAccelerationAuthorityFactPacket> AuthorityAcceleration;
    /** 装备选择输入槽位 */
    TBBBCharacterInputSlot<FBBBItemAddLocalControlPacket> ItemAdd;
    /** 背包移动输入槽位 */
    TBBBCharacterInputSlot<FBBBItemMoveLocalControlPacket> ItemMove;
    /** 远端已经成立的装备持有关系 */
    TBBBCharacterInputSlot<FBBBEquipmentSelectionRemoteMessagePacket> RemoteEquipmentSelectionState;

    /** 权威装备选择事实输入槽位 */
    TBBBCharacterInputSlot<FBBBEquipmentSelectionAuthorityFactPacket> AuthorityEquipmentSelectionState;
    /** 快捷栏装备选择输入槽位 */
    TBBBCharacterInputSlot<FBBBItemSelectLocalControlPacket> ItemSelect;
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
    /** 攀爬状态专用蒙太奇输入槽位 */
    TBBBCharacterInputSlot<FBBBTraversalMontageLocalControlPacket> TraversalMontage;
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

    /** 本机额外瞄准冲击输入槽位 */
    TBBBCharacterInputSlot<FBBBAimImpulseLocalControlPacket> AimImpulse;

    /** 还原额外瞄准冲击输入槽位 */
    TBBBCharacterInputSlot<FBBBAimImpulseAuthorityFactPacket> AuthorityAimImpulse;
};

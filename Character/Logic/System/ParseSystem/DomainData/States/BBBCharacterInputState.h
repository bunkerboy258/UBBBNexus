#pragma once
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterEquipmentShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterEquipmentHideLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Animation/FBBBCharacterFireIntentLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterMeleeEndLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterMeleeBeginLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterQuickMeleeLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Interaction/FBBBCharacterUseAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Interaction/FBBBCharacterUseRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRecoveryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBCharacterItemConsumeLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBCharacterItemQuickUseLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionReleaseLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionFinishLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Appearance/FBBBCharacterAppearanceColorLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Appearance/FBBBCharacterAppearanceCamouflageLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Appearance/FBBBCharacterAppearanceBaseLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Appearance/FBBBCharacterAppearanceDirtLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Appearance/FBBBCharacterAppearanceWeatheringLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Appearance/FBBBCharacterAppearanceRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Appearance/FBBBCharacterAppearanceAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Interaction/FBBBCharacterInteractionSelectionRemoteMessagePacket.h"

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Life/FBBBCharacterLifeAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterLifeRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterDamageDeliveryRemoteMessagePacket.h"
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
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBCharacterItemAddLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBCharacterItemMoveLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Interaction/FBBBCharacterInteractionSelectionAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBFullBodyMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBUpperBodyMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBFullBodyAdditivePreAimMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBUpperBodyAdditiveMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Animation/FBBBAdditiveHitReactMontageAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Item/FBBBCharacterItemSelectLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Traversal/FBBBTraversalStartRemoteMessagePacket.h"

#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Traversal/FBBBTraversalEndRemoteMessagePacket.h"

#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Traversal/FBBBTraversalStartAuthorityFactPacket.h"

#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Traversal/FBBBTraversalEndAuthorityFactPacket.h"

#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Interaction/FBBBCharacterInteractionEndContactLocalControlPacket.h"

#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueBeginLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueCancelLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueRequestLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterRescueRequestRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueEndLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterRescueEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/LocalControl/Life/FBBBCharacterRescueReplyLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterRescueReplyRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/RemoteMessage/Life/FBBBCharacterRescueSnapshotRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Character/Input/AuthorityFact/Life/FBBBCharacterRescueSnapshotAuthorityFactPacket.h"

/** 角色全部输入的固定槽位状态 */
struct FBBBCharacterInputState final
{
    /** 角色表现使用的开火意图快照输入 */
    TBBBCharacterInputSlot<FBBBCharacterFireIntentLocalControlPacket> FireIntent;
    /** 快速近战输入 */
    TBBBCharacterInputSlot<FBBBCharacterMeleeEndLocalControlPacket> MeleeEnd;
    /** 快速近战输入 */
    TBBBCharacterInputSlot<FBBBCharacterMeleeBeginLocalControlPacket> MeleeBegin;
    /** 快速近战输入 */
    TBBBCharacterInputSlot<FBBBCharacterQuickMeleeLocalControlPacket> QuickMelee;
    /** 当前使用结果输入 */
    TBBBCharacterInputSlot<FBBBCharacterUseAuthorityFactPacket> UseAuthorityFact;
    /** 当前使用结果输入 */
    TBBBCharacterInputSlot<FBBBCharacterUseRemoteMessagePacket> UseRemoteMessage;
    /** Recovery 固定输入 */
    TBBBCharacterInputSlot<FBBBCharacterRecoveryLocalControlPacket> Recovery;
    /** ItemConsume 固定输入 */
    TBBBCharacterInputSlot<FBBBCharacterItemConsumeLocalControlPacket> ItemConsume;
    /** ItemQuickUse 固定输入 */
    TBBBCharacterInputSlot<FBBBCharacterItemQuickUseLocalControlPacket> ItemQuickUse;
    /** InteractionPrimary 固定输入 */
    TBBBCharacterInputSlot<FBBBCharacterInteractionPrimaryLocalControlPacket> InteractionPrimary;
    /** InteractionSecondary 固定输入 */
    TBBBCharacterInputSlot<FBBBCharacterInteractionSecondaryLocalControlPacket> InteractionSecondary;
    /** InteractionRelease 固定输入 */
    TBBBCharacterInputSlot<FBBBCharacterInteractionReleaseLocalControlPacket> InteractionRelease;
    /** InteractionFinish 固定输入 */
    TBBBCharacterInputSlot<FBBBCharacterInteractionFinishLocalControlPacket> InteractionFinish;
    /** 接收的生命结果 */
    TBBBCharacterInputSlot<FBBBCharacterLifeAuthorityFactPacket> AuthorityLife;
    /** 控制者生成的生命结果 */
    TBBBCharacterInputSlot<FBBBCharacterLifeRemoteMessagePacket> RemoteLife;
    /** 需向控制者投送的独立命中 */
    TBBBCharacterInputSlot<FBBBCharacterDamageDeliveryRemoteMessagePacket> DamageDelivery;
    /** 装备独立使用结果输入 */
    /** 装备独立使用结果输入 */
    /** 本帧全部独立伤害输入 */
    TBBBCharacterInputSlot<FBBBCharacterDamageLocalControlPacket> Damage;
    /** 人物动画显示装备输入 */
    TBBBCharacterInputSlot<FBBBCharacterEquipmentShowLocalControlPacket> EquipmentShow;
    /** 人物动画隐藏装备输入 */
    TBBBCharacterInputSlot<FBBBCharacterEquipmentHideLocalControlPacket> EquipmentHide;
    /** 装备BeginAction通知输入槽 */
    TBBBCharacterInputSlot<FBBBCharacterInteractionBeginActionLocalControlPacket> EquipmentBeginAction;
    /** 装备EndAction通知输入槽 */
    TBBBCharacterInputSlot<FBBBCharacterInteractionEndActionLocalControlPacket> EquipmentEndAction;
    /** 装备BeginContact通知输入槽 */
    TBBBCharacterInputSlot<FBBBCharacterInteractionBeginContactLocalControlPacket> EquipmentBeginContact;
    /** 装备EndContact通知输入槽 */
    TBBBCharacterInputSlot<FBBBCharacterInteractionEndContactLocalControlPacket> EquipmentEndContact;
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
    TBBBCharacterInputSlot<FBBBCharacterItemAddLocalControlPacket> ItemAdd;
    /** 背包移动输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterItemMoveLocalControlPacket> ItemMove;
    /** 远端已经成立的装备持有关系 */
    TBBBCharacterInputSlot<FBBBCharacterInteractionSelectionRemoteMessagePacket> RemoteInteractionSelectionState;

    /** 权威装备选择事实输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterInteractionSelectionAuthorityFactPacket> AuthorityInteractionSelectionState;
    /** 快捷栏装备选择输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterItemSelectLocalControlPacket> ItemSelect;
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
    TBBBCharacterInputSlot<FBBBAimImpulseAuthorityFactPacket> AuthorityAimImpulse;    /** 救援输入固定槽位 */
    TBBBCharacterInputSlot<FBBBCharacterRescueBeginLocalControlPacket> RescueBeginLocalControl;
    /** 救援输入固定槽位 */
    TBBBCharacterInputSlot<FBBBCharacterRescueCancelLocalControlPacket> RescueCancelLocalControl;
    /** 救援输入固定槽位 */
    TBBBCharacterInputSlot<FBBBCharacterRescueRequestLocalControlPacket> RescueRequestLocalControl;
    /** 救援输入固定槽位 */
    TBBBCharacterInputSlot<FBBBCharacterRescueRequestRemoteMessagePacket> RescueRequestRemoteMessage;
    /** 救援输入固定槽位 */
    TBBBCharacterInputSlot<FBBBCharacterRescueEndLocalControlPacket> RescueEndLocalControl;
    /** 救援输入固定槽位 */
    TBBBCharacterInputSlot<FBBBCharacterRescueEndRemoteMessagePacket> RescueEndRemoteMessage;
    /** 救援输入固定槽位 */
    TBBBCharacterInputSlot<FBBBCharacterRescueReplyLocalControlPacket> RescueReplyLocalControl;
    /** 救援输入固定槽位 */
    TBBBCharacterInputSlot<FBBBCharacterRescueReplyRemoteMessagePacket> RescueReplyRemoteMessage;
    /** 救援输入固定槽位 */
    TBBBCharacterInputSlot<FBBBCharacterRescueSnapshotRemoteMessagePacket> RescueSnapshotRemoteMessage;
    /** 救援输入固定槽位 */
    TBBBCharacterInputSlot<FBBBCharacterRescueSnapshotAuthorityFactPacket> RescueSnapshotAuthorityFact;

    /** 外观固定输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterAppearanceColorLocalControlPacket> AppearanceColorLocalControl;
    /** 外观固定输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterAppearanceCamouflageLocalControlPacket> AppearanceCamouflageLocalControl;
    /** 外观固定输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterAppearanceBaseLocalControlPacket> AppearanceBaseLocalControl;
    /** 外观固定输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterAppearanceDirtLocalControlPacket> AppearanceDirtLocalControl;
    /** 外观固定输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterAppearanceWeatheringLocalControlPacket> AppearanceWeatheringLocalControl;
    /** 外观固定输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterAppearanceRemoteMessagePacket> AppearanceRemoteMessage;
    /** 外观固定输入槽位 */
    TBBBCharacterInputSlot<FBBBCharacterAppearanceAuthorityFactPacket> AppearanceAuthorityFact;
};

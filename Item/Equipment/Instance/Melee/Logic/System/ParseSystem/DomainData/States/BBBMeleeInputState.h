#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Reload/FBBBMeleeLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Reload/FBBBMeleeInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Fire/FBBBMeleeBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Fire/FBBBMeleeAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleeSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/AuthorityFact/Equipment/FBBBMeleeVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/RemoteMessage/Equipment/FBBBMeleeVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleeShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleeHideLocalControlPacket.h"

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Logic/System/ParseSystem/DomainData/Definitions/BBBMeleeInputSlot.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleeEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/AuthorityFact/Equipment/FBBBMeleeEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleePrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleeBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleeBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleeEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleeEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/RemoteMessage/Action/FBBBMeleeAttackStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/RemoteMessage/Action/FBBBMeleeAttackEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/AuthorityFact/Action/FBBBMeleeAttackStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Melee/Input/AuthorityFact/Action/FBBBMeleeAttackEndAuthorityFactPacket.h"

/** 近战请求与动画通知的固定槽位 */
struct FBBBMeleeInputState final
{
    /** LoadAmmoLocalControl固定输入槽 */
    TBBBMeleeInputSlot<FBBBMeleeLoadAmmoLocalControlPacket> LoadAmmoLocalControl;

    /** InterruptReloadLocalControl固定输入槽 */
    TBBBMeleeInputSlot<FBBBMeleeInterruptReloadLocalControlPacket> InterruptReloadLocalControl;

    /** BlockFireLocalControl固定输入槽 */
    TBBBMeleeInputSlot<FBBBMeleeBlockFireLocalControlPacket> BlockFireLocalControl;

    /** AllowFireLocalControl固定输入槽 */
    TBBBMeleeInputSlot<FBBBMeleeAllowFireLocalControlPacket> AllowFireLocalControl;

    /** SecondaryLocalControl固定输入槽 */
    TBBBMeleeInputSlot<FBBBMeleeSecondaryLocalControlPacket> SecondaryLocalControl;

    /** 网格显示输入槽位 */
    TBBBMeleeInputSlot<FBBBMeleeVisibilityAuthorityFactPacket> VisibilityAuthorityFact;
    /** 网格显示输入槽位 */
    TBBBMeleeInputSlot<FBBBMeleeVisibilityRemoteMessagePacket> VisibilityRemoteMessage;
    /** 网格显示输入槽位 */
    TBBBMeleeInputSlot<FBBBMeleeShowLocalControlPacket> ShowLocalControl;
    /** 网格显示输入槽位 */
    TBBBMeleeInputSlot<FBBBMeleeHideLocalControlPacket> HideLocalControl;
    /** Equip固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleeEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleeEquipAuthorityFactPacket> AuthorityEquip;

    /** Attack固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleePrimaryLocalControlPacket> Attack;

    /** BeginAction固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleeBeginActionLocalControlPacket> BeginAction;

    /** BeginContact固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleeBeginContactLocalControlPacket> BeginContact;

    /** EndContact固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleeEndContactLocalControlPacket> EndContact;

    /** EndAction固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleeEndActionLocalControlPacket> EndAction;

    /** RemoteAttackStart固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleeAttackStartRemoteMessagePacket> RemoteAttackStart;

    /** RemoteAttackEnd固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleeAttackEndRemoteMessagePacket> RemoteAttackEnd;

    /** AuthorityAttackStart固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleeAttackStartAuthorityFactPacket> AuthorityAttackStart;

    /** AuthorityAttackEnd固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleeAttackEndAuthorityFactPacket> AuthorityAttackEnd;

private:
    friend struct FBBBMeleeParseDomainState;
    FBBBMeleeInputState() = default;
};

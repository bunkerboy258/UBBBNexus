#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Logic/System/ParseSystem/DomainData/Definitions/BBBMeleeInputSlot.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleeEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/AuthorityFact/Equipment/FBBBMeleeEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/LocalControl/Action/FBBBMeleeAttackLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/LocalControl/Equipment/FBBBMeleeActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/LocalControl/Action/FBBBMeleeBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/LocalControl/Action/FBBBMeleeBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/LocalControl/Action/FBBBMeleeEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/LocalControl/Action/FBBBMeleeEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/RemoteMessage/Action/FBBBMeleeAttackStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/RemoteMessage/Action/FBBBMeleeAttackEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/AuthorityFact/Action/FBBBMeleeAttackStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Melee/Input/AuthorityFact/Action/FBBBMeleeAttackEndAuthorityFactPacket.h"

/** 近战请求与动画通知的固定槽位 */
struct FBBBMeleeInputState final
{
    /** Equip固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleeEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleeEquipAuthorityFactPacket> AuthorityEquip;

    /** Attack固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleeAttackLocalControlPacket> Attack;

    /** ActionPermission固定槽位 */
    TBBBMeleeInputSlot<FBBBMeleeActionPermissionLocalControlPacket> ActionPermission;

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

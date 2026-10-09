#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Logic/System/ParseSystem/DomainData/Definitions/BBBRifleInputSlot.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/AuthorityFact/Equipment/FBBBRifleEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Fire/FBBBRifleFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Reload/FBBBRifleReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Fire/FBBBRifleBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Fire/FBBBRifleAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Reload/FBBBRifleLoadMagazineLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Reload/FBBBRifleInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/RemoteMessage/Fire/FBBBRifleFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/RemoteMessage/Reload/FBBBRifleReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/RemoteMessage/Reload/FBBBRifleReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/AuthorityFact/Fire/FBBBRifleFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/AuthorityFact/Reload/FBBBRifleReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Rifle/Input/AuthorityFact/Reload/FBBBRifleReloadEndAuthorityFactPacket.h"

/** 步枪等待更新时消费的固定输入槽 */
struct FBBBRifleInputState final
{
    /** Equip固定槽位 */
    TBBBRifleInputSlot<FBBBRifleEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBRifleInputSlot<FBBBRifleEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBRifleInputSlot<FBBBRifleFireLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBRifleInputSlot<FBBBRifleReloadLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBRifleInputSlot<FBBBRifleBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBRifleInputSlot<FBBBRifleAllowFireLocalControlPacket> AllowFire;

    /** LoadMagazine固定槽位 */
    TBBBRifleInputSlot<FBBBRifleLoadMagazineLocalControlPacket> LoadMagazine;

    /** InterruptReload固定槽位 */
    TBBBRifleInputSlot<FBBBRifleInterruptReloadLocalControlPacket> InterruptReload;

    /** ActionPermission固定槽位 */
    TBBBRifleInputSlot<FBBBRifleActionPermissionLocalControlPacket> ActionPermission;

    /** RemoteFire固定槽位 */
    TBBBRifleInputSlot<FBBBRifleFireRemoteMessagePacket> RemoteFire;

    /** RemoteReloadStart固定槽位 */
    TBBBRifleInputSlot<FBBBRifleReloadStartRemoteMessagePacket> RemoteReloadStart;

    /** RemoteReloadEnd固定槽位 */
    TBBBRifleInputSlot<FBBBRifleReloadEndRemoteMessagePacket> RemoteReloadEnd;

    /** AuthorityFire固定槽位 */
    TBBBRifleInputSlot<FBBBRifleFireAuthorityFactPacket> AuthorityFire;

    /** AuthorityReloadStart固定槽位 */
    TBBBRifleInputSlot<FBBBRifleReloadStartAuthorityFactPacket> AuthorityReloadStart;

    /** AuthorityReloadEnd固定槽位 */
    TBBBRifleInputSlot<FBBBRifleReloadEndAuthorityFactPacket> AuthorityReloadEnd;

private:
    friend struct FBBBRifleParseDomainState;
    FBBBRifleInputState() = default;
};

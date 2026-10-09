#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Logic/System/ParseSystem/DomainData/Definitions/BBBShotgunInputSlot.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/AuthorityFact/Equipment/FBBBShotgunEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/LocalControl/Fire/FBBBShotgunFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/LocalControl/Reload/FBBBShotgunReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/LocalControl/Fire/FBBBShotgunBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/LocalControl/Fire/FBBBShotgunAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/LocalControl/Reload/FBBBShotgunLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/LocalControl/Reload/FBBBShotgunInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/RemoteMessage/Fire/FBBBShotgunFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/RemoteMessage/Reload/FBBBShotgunReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/RemoteMessage/Reload/FBBBShotgunReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/AuthorityFact/Fire/FBBBShotgunFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/AuthorityFact/Reload/FBBBShotgunReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Shotgun/Input/AuthorityFact/Reload/FBBBShotgunReloadEndAuthorityFactPacket.h"

/** 霰弹枪等待更新时消费的固定输入槽 */
struct FBBBShotgunInputState final
{
    /** Equip固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunFireLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunReloadLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunAllowFireLocalControlPacket> AllowFire;

    /** LoadAmmo固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunLoadAmmoLocalControlPacket> LoadAmmo;

    /** InterruptReload固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunInterruptReloadLocalControlPacket> InterruptReload;

    /** ActionPermission固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunActionPermissionLocalControlPacket> ActionPermission;

    /** RemoteFire固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunFireRemoteMessagePacket> RemoteFire;

    /** RemoteReloadStart固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunReloadStartRemoteMessagePacket> RemoteReloadStart;

    /** RemoteReloadEnd固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunReloadEndRemoteMessagePacket> RemoteReloadEnd;

    /** AuthorityFire固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunFireAuthorityFactPacket> AuthorityFire;

    /** AuthorityReloadStart固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunReloadStartAuthorityFactPacket> AuthorityReloadStart;

    /** AuthorityReloadEnd固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunReloadEndAuthorityFactPacket> AuthorityReloadEnd;

private:
    friend struct FBBBShotgunParseDomainState;
    FBBBShotgunInputState() = default;
};

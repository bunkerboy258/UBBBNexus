#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Logic/System/ParseSystem/DomainData/Definitions/BBBRevolverInputSlot.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/AuthorityFact/Equipment/FBBBRevolverEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/LocalControl/Fire/FBBBRevolverFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/LocalControl/Reload/FBBBRevolverReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/LocalControl/Fire/FBBBRevolverBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/LocalControl/Fire/FBBBRevolverAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/LocalControl/Reload/FBBBRevolverLoadMagazineLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/LocalControl/Reload/FBBBRevolverInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/RemoteMessage/Fire/FBBBRevolverFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/RemoteMessage/Reload/FBBBRevolverReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/RemoteMessage/Reload/FBBBRevolverReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/AuthorityFact/Fire/FBBBRevolverFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/AuthorityFact/Reload/FBBBRevolverReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Revolver/Input/AuthorityFact/Reload/FBBBRevolverReloadEndAuthorityFactPacket.h"

/** 左轮等待更新时消费的固定输入槽 */
struct FBBBRevolverInputState final
{
    /** Equip固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverFireLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverReloadLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverAllowFireLocalControlPacket> AllowFire;

    /** LoadMagazine固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverLoadMagazineLocalControlPacket> LoadMagazine;

    /** InterruptReload固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverInterruptReloadLocalControlPacket> InterruptReload;

    /** ActionPermission固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverActionPermissionLocalControlPacket> ActionPermission;

    /** RemoteFire固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverFireRemoteMessagePacket> RemoteFire;

    /** RemoteReloadStart固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverReloadStartRemoteMessagePacket> RemoteReloadStart;

    /** RemoteReloadEnd固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverReloadEndRemoteMessagePacket> RemoteReloadEnd;

    /** AuthorityFire固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverFireAuthorityFactPacket> AuthorityFire;

    /** AuthorityReloadStart固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverReloadStartAuthorityFactPacket> AuthorityReloadStart;

    /** AuthorityReloadEnd固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverReloadEndAuthorityFactPacket> AuthorityReloadEnd;

private:
    friend struct FBBBRevolverParseDomainState;
    FBBBRevolverInputState() = default;
};

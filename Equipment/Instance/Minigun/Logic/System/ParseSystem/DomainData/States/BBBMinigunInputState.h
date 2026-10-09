#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Logic/System/ParseSystem/DomainData/Definitions/BBBMinigunInputSlot.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/AuthorityFact/Equipment/FBBBMinigunEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/LocalControl/Fire/FBBBMinigunFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/LocalControl/Reload/FBBBMinigunReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/LocalControl/Fire/FBBBMinigunBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/LocalControl/Fire/FBBBMinigunAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/LocalControl/Reload/FBBBMinigunLoadMagazineLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/LocalControl/Reload/FBBBMinigunInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/RemoteMessage/Fire/FBBBMinigunFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/RemoteMessage/Reload/FBBBMinigunReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/RemoteMessage/Reload/FBBBMinigunReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/AuthorityFact/Fire/FBBBMinigunFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/AuthorityFact/Reload/FBBBMinigunReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Minigun/Input/AuthorityFact/Reload/FBBBMinigunReloadEndAuthorityFactPacket.h"

/** 转管机枪等待更新时消费的固定输入槽 */
struct FBBBMinigunInputState final
{
    /** Equip固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunFireLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunReloadLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunAllowFireLocalControlPacket> AllowFire;

    /** LoadMagazine固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunLoadMagazineLocalControlPacket> LoadMagazine;

    /** InterruptReload固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunInterruptReloadLocalControlPacket> InterruptReload;

    /** ActionPermission固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunActionPermissionLocalControlPacket> ActionPermission;

    /** RemoteFire固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunFireRemoteMessagePacket> RemoteFire;

    /** RemoteReloadStart固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunReloadStartRemoteMessagePacket> RemoteReloadStart;

    /** RemoteReloadEnd固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunReloadEndRemoteMessagePacket> RemoteReloadEnd;

    /** AuthorityFire固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunFireAuthorityFactPacket> AuthorityFire;

    /** AuthorityReloadStart固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunReloadStartAuthorityFactPacket> AuthorityReloadStart;

    /** AuthorityReloadEnd固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunReloadEndAuthorityFactPacket> AuthorityReloadEnd;

private:
    friend struct FBBBMinigunParseDomainState;
    FBBBMinigunInputState() = default;
};

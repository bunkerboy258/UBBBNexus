#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Logic/System/ParseSystem/DomainData/Definitions/BBBPistolInputSlot.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/AuthorityFact/Equipment/FBBBPistolEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/LocalControl/Fire/FBBBPistolFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/LocalControl/Reload/FBBBPistolReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/LocalControl/Fire/FBBBPistolBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/LocalControl/Fire/FBBBPistolAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/LocalControl/Reload/FBBBPistolLoadMagazineLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/LocalControl/Reload/FBBBPistolInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/RemoteMessage/Fire/FBBBPistolFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/RemoteMessage/Reload/FBBBPistolReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/RemoteMessage/Reload/FBBBPistolReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/AuthorityFact/Fire/FBBBPistolFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/AuthorityFact/Reload/FBBBPistolReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Pistol/Input/AuthorityFact/Reload/FBBBPistolReloadEndAuthorityFactPacket.h"

/** 手枪等待更新时消费的固定输入槽 */
struct FBBBPistolInputState final
{
    /** Equip固定槽位 */
    TBBBPistolInputSlot<FBBBPistolEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBPistolInputSlot<FBBBPistolEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBPistolInputSlot<FBBBPistolFireLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBPistolInputSlot<FBBBPistolReloadLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBPistolInputSlot<FBBBPistolBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBPistolInputSlot<FBBBPistolAllowFireLocalControlPacket> AllowFire;

    /** LoadMagazine固定槽位 */
    TBBBPistolInputSlot<FBBBPistolLoadMagazineLocalControlPacket> LoadMagazine;

    /** InterruptReload固定槽位 */
    TBBBPistolInputSlot<FBBBPistolInterruptReloadLocalControlPacket> InterruptReload;

    /** ActionPermission固定槽位 */
    TBBBPistolInputSlot<FBBBPistolActionPermissionLocalControlPacket> ActionPermission;

    /** RemoteFire固定槽位 */
    TBBBPistolInputSlot<FBBBPistolFireRemoteMessagePacket> RemoteFire;

    /** RemoteReloadStart固定槽位 */
    TBBBPistolInputSlot<FBBBPistolReloadStartRemoteMessagePacket> RemoteReloadStart;

    /** RemoteReloadEnd固定槽位 */
    TBBBPistolInputSlot<FBBBPistolReloadEndRemoteMessagePacket> RemoteReloadEnd;

    /** AuthorityFire固定槽位 */
    TBBBPistolInputSlot<FBBBPistolFireAuthorityFactPacket> AuthorityFire;

    /** AuthorityReloadStart固定槽位 */
    TBBBPistolInputSlot<FBBBPistolReloadStartAuthorityFactPacket> AuthorityReloadStart;

    /** AuthorityReloadEnd固定槽位 */
    TBBBPistolInputSlot<FBBBPistolReloadEndAuthorityFactPacket> AuthorityReloadEnd;

private:
    friend struct FBBBPistolParseDomainState;
    FBBBPistolInputState() = default;
};

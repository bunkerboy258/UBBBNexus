#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Logic/System/ParseSystem/DomainData/Definitions/BBBSniperInputSlot.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/LocalControl/Equipment/FBBBSniperEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/AuthorityFact/Equipment/FBBBSniperEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/LocalControl/Fire/FBBBSniperFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/LocalControl/Reload/FBBBSniperReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/LocalControl/Fire/FBBBSniperBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/LocalControl/Fire/FBBBSniperAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/LocalControl/Reload/FBBBSniperLoadMagazineLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/LocalControl/Reload/FBBBSniperInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/LocalControl/Equipment/FBBBSniperActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/RemoteMessage/Fire/FBBBSniperFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/RemoteMessage/Reload/FBBBSniperReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/RemoteMessage/Reload/FBBBSniperReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/AuthorityFact/Fire/FBBBSniperFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/AuthorityFact/Reload/FBBBSniperReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/Sniper/Input/AuthorityFact/Reload/FBBBSniperReloadEndAuthorityFactPacket.h"

/** 狙击枪等待更新时消费的固定输入槽 */
struct FBBBSniperInputState final
{
    /** Equip固定槽位 */
    TBBBSniperInputSlot<FBBBSniperEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBSniperInputSlot<FBBBSniperEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBSniperInputSlot<FBBBSniperFireLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBSniperInputSlot<FBBBSniperReloadLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBSniperInputSlot<FBBBSniperBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBSniperInputSlot<FBBBSniperAllowFireLocalControlPacket> AllowFire;

    /** LoadMagazine固定槽位 */
    TBBBSniperInputSlot<FBBBSniperLoadMagazineLocalControlPacket> LoadMagazine;

    /** InterruptReload固定槽位 */
    TBBBSniperInputSlot<FBBBSniperInterruptReloadLocalControlPacket> InterruptReload;

    /** ActionPermission固定槽位 */
    TBBBSniperInputSlot<FBBBSniperActionPermissionLocalControlPacket> ActionPermission;

    /** RemoteFire固定槽位 */
    TBBBSniperInputSlot<FBBBSniperFireRemoteMessagePacket> RemoteFire;

    /** RemoteReloadStart固定槽位 */
    TBBBSniperInputSlot<FBBBSniperReloadStartRemoteMessagePacket> RemoteReloadStart;

    /** RemoteReloadEnd固定槽位 */
    TBBBSniperInputSlot<FBBBSniperReloadEndRemoteMessagePacket> RemoteReloadEnd;

    /** AuthorityFire固定槽位 */
    TBBBSniperInputSlot<FBBBSniperFireAuthorityFactPacket> AuthorityFire;

    /** AuthorityReloadStart固定槽位 */
    TBBBSniperInputSlot<FBBBSniperReloadStartAuthorityFactPacket> AuthorityReloadStart;

    /** AuthorityReloadEnd固定槽位 */
    TBBBSniperInputSlot<FBBBSniperReloadEndAuthorityFactPacket> AuthorityReloadEnd;

private:
    friend struct FBBBSniperParseDomainState;
    FBBBSniperInputState() = default;
};

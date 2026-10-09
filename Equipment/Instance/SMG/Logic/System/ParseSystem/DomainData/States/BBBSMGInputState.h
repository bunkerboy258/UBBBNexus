#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Logic/System/ParseSystem/DomainData/Definitions/BBBSMGInputSlot.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/LocalControl/Equipment/FBBBSMGEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/AuthorityFact/Equipment/FBBBSMGEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/LocalControl/Fire/FBBBSMGFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/LocalControl/Reload/FBBBSMGReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/LocalControl/Fire/FBBBSMGBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/LocalControl/Fire/FBBBSMGAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/LocalControl/Reload/FBBBSMGLoadMagazineLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/LocalControl/Reload/FBBBSMGInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/LocalControl/Equipment/FBBBSMGActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/RemoteMessage/Fire/FBBBSMGFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/RemoteMessage/Reload/FBBBSMGReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/RemoteMessage/Reload/FBBBSMGReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/AuthorityFact/Fire/FBBBSMGFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/AuthorityFact/Reload/FBBBSMGReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/SMG/Input/AuthorityFact/Reload/FBBBSMGReloadEndAuthorityFactPacket.h"

/** 冲锋枪等待更新时消费的固定输入槽 */
struct FBBBSMGInputState final
{
    /** Equip固定槽位 */
    TBBBSMGInputSlot<FBBBSMGEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBSMGInputSlot<FBBBSMGEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBSMGInputSlot<FBBBSMGFireLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBSMGInputSlot<FBBBSMGReloadLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBSMGInputSlot<FBBBSMGBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBSMGInputSlot<FBBBSMGAllowFireLocalControlPacket> AllowFire;

    /** LoadMagazine固定槽位 */
    TBBBSMGInputSlot<FBBBSMGLoadMagazineLocalControlPacket> LoadMagazine;

    /** InterruptReload固定槽位 */
    TBBBSMGInputSlot<FBBBSMGInterruptReloadLocalControlPacket> InterruptReload;

    /** ActionPermission固定槽位 */
    TBBBSMGInputSlot<FBBBSMGActionPermissionLocalControlPacket> ActionPermission;

    /** RemoteFire固定槽位 */
    TBBBSMGInputSlot<FBBBSMGFireRemoteMessagePacket> RemoteFire;

    /** RemoteReloadStart固定槽位 */
    TBBBSMGInputSlot<FBBBSMGReloadStartRemoteMessagePacket> RemoteReloadStart;

    /** RemoteReloadEnd固定槽位 */
    TBBBSMGInputSlot<FBBBSMGReloadEndRemoteMessagePacket> RemoteReloadEnd;

    /** AuthorityFire固定槽位 */
    TBBBSMGInputSlot<FBBBSMGFireAuthorityFactPacket> AuthorityFire;

    /** AuthorityReloadStart固定槽位 */
    TBBBSMGInputSlot<FBBBSMGReloadStartAuthorityFactPacket> AuthorityReloadStart;

    /** AuthorityReloadEnd固定槽位 */
    TBBBSMGInputSlot<FBBBSMGReloadEndAuthorityFactPacket> AuthorityReloadEnd;

private:
    friend struct FBBBSMGParseDomainState;
    FBBBSMGInputState() = default;
};

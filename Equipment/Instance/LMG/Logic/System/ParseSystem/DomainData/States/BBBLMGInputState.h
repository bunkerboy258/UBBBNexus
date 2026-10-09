#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Logic/System/ParseSystem/DomainData/Definitions/BBBLMGInputSlot.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/AuthorityFact/Equipment/FBBBLMGEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/LocalControl/Fire/FBBBLMGFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/LocalControl/Reload/FBBBLMGReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/LocalControl/Fire/FBBBLMGBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/LocalControl/Fire/FBBBLMGAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/LocalControl/Reload/FBBBLMGLoadMagazineLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/LocalControl/Reload/FBBBLMGInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGActionPermissionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/RemoteMessage/Fire/FBBBLMGFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/RemoteMessage/Reload/FBBBLMGReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/RemoteMessage/Reload/FBBBLMGReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/AuthorityFact/Fire/FBBBLMGFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/AuthorityFact/Reload/FBBBLMGReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Equipment/Instance/LMG/Input/AuthorityFact/Reload/FBBBLMGReloadEndAuthorityFactPacket.h"

/** 轻机枪等待更新时消费的固定输入槽 */
struct FBBBLMGInputState final
{
    /** Equip固定槽位 */
    TBBBLMGInputSlot<FBBBLMGEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBLMGInputSlot<FBBBLMGEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBLMGInputSlot<FBBBLMGFireLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBLMGInputSlot<FBBBLMGReloadLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBLMGInputSlot<FBBBLMGBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBLMGInputSlot<FBBBLMGAllowFireLocalControlPacket> AllowFire;

    /** LoadMagazine固定槽位 */
    TBBBLMGInputSlot<FBBBLMGLoadMagazineLocalControlPacket> LoadMagazine;

    /** InterruptReload固定槽位 */
    TBBBLMGInputSlot<FBBBLMGInterruptReloadLocalControlPacket> InterruptReload;

    /** ActionPermission固定槽位 */
    TBBBLMGInputSlot<FBBBLMGActionPermissionLocalControlPacket> ActionPermission;

    /** RemoteFire固定槽位 */
    TBBBLMGInputSlot<FBBBLMGFireRemoteMessagePacket> RemoteFire;

    /** RemoteReloadStart固定槽位 */
    TBBBLMGInputSlot<FBBBLMGReloadStartRemoteMessagePacket> RemoteReloadStart;

    /** RemoteReloadEnd固定槽位 */
    TBBBLMGInputSlot<FBBBLMGReloadEndRemoteMessagePacket> RemoteReloadEnd;

    /** AuthorityFire固定槽位 */
    TBBBLMGInputSlot<FBBBLMGFireAuthorityFactPacket> AuthorityFire;

    /** AuthorityReloadStart固定槽位 */
    TBBBLMGInputSlot<FBBBLMGReloadStartAuthorityFactPacket> AuthorityReloadStart;

    /** AuthorityReloadEnd固定槽位 */
    TBBBLMGInputSlot<FBBBLMGReloadEndAuthorityFactPacket> AuthorityReloadEnd;

private:
    friend struct FBBBLMGParseDomainState;
    FBBBLMGInputState() = default;
};

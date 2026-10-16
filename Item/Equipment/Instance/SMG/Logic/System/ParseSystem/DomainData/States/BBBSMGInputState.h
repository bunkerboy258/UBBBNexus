#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/LocalControl/Equipment/FBBBSMGEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/LocalControl/Equipment/FBBBSMGEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/LocalControl/Equipment/FBBBSMGBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/LocalControl/Equipment/FBBBSMGBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/AuthorityFact/Equipment/FBBBSMGVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/RemoteMessage/Equipment/FBBBSMGVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/LocalControl/Equipment/FBBBSMGShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/LocalControl/Equipment/FBBBSMGHideLocalControlPacket.h"

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Logic/System/ParseSystem/DomainData/Definitions/BBBSMGInputSlot.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/LocalControl/Equipment/FBBBSMGEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/AuthorityFact/Equipment/FBBBSMGEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/LocalControl/Equipment/FBBBSMGPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/LocalControl/Equipment/FBBBSMGSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/LocalControl/Fire/FBBBSMGBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/LocalControl/Fire/FBBBSMGAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/LocalControl/Reload/FBBBSMGLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/LocalControl/Reload/FBBBSMGInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/RemoteMessage/Fire/FBBBSMGFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/RemoteMessage/Reload/FBBBSMGReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/RemoteMessage/Reload/FBBBSMGReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/AuthorityFact/Fire/FBBBSMGFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/AuthorityFact/Reload/FBBBSMGReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/SMG/Input/AuthorityFact/Reload/FBBBSMGReloadEndAuthorityFactPacket.h"

/** 冲锋枪等待更新时消费的固定输入槽 */
struct FBBBSMGInputState final
{
    /** EndContactLocalControl固定输入槽 */
    TBBBSMGInputSlot<FBBBSMGEndContactLocalControlPacket> EndContactLocalControl;

    /** EndActionLocalControl固定输入槽 */
    TBBBSMGInputSlot<FBBBSMGEndActionLocalControlPacket> EndActionLocalControl;

    /** BeginContactLocalControl固定输入槽 */
    TBBBSMGInputSlot<FBBBSMGBeginContactLocalControlPacket> BeginContactLocalControl;

    /** BeginActionLocalControl固定输入槽 */
    TBBBSMGInputSlot<FBBBSMGBeginActionLocalControlPacket> BeginActionLocalControl;

    /** 网格显示输入槽位 */
    TBBBSMGInputSlot<FBBBSMGVisibilityAuthorityFactPacket> VisibilityAuthorityFact;
    /** 网格显示输入槽位 */
    TBBBSMGInputSlot<FBBBSMGVisibilityRemoteMessagePacket> VisibilityRemoteMessage;
    /** 网格显示输入槽位 */
    TBBBSMGInputSlot<FBBBSMGShowLocalControlPacket> ShowLocalControl;
    /** 网格显示输入槽位 */
    TBBBSMGInputSlot<FBBBSMGHideLocalControlPacket> HideLocalControl;
    /** Equip固定槽位 */
    TBBBSMGInputSlot<FBBBSMGEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBSMGInputSlot<FBBBSMGEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBSMGInputSlot<FBBBSMGPrimaryLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBSMGInputSlot<FBBBSMGSecondaryLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBSMGInputSlot<FBBBSMGBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBSMGInputSlot<FBBBSMGAllowFireLocalControlPacket> AllowFire;

    /** LoadMagazine固定槽位 */
    TBBBSMGInputSlot<FBBBSMGLoadAmmoLocalControlPacket> LoadMagazine;

    /** InterruptReload固定槽位 */
    TBBBSMGInputSlot<FBBBSMGInterruptReloadLocalControlPacket> InterruptReload;

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

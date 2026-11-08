#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/LocalControl/Equipment/FBBBSniperEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/LocalControl/Equipment/FBBBSniperEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/LocalControl/Equipment/FBBBSniperBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/LocalControl/Equipment/FBBBSniperBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/AuthorityFact/Equipment/FBBBSniperVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/RemoteMessage/Equipment/FBBBSniperVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/LocalControl/Equipment/FBBBSniperShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/LocalControl/Equipment/FBBBSniperHideLocalControlPacket.h"

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Logic/System/ParseSystem/DomainData/Definitions/BBBSniperInputSlot.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/LocalControl/Equipment/FBBBSniperEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/AuthorityFact/Equipment/FBBBSniperEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/LocalControl/Equipment/FBBBSniperPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/LocalControl/Equipment/FBBBSniperSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/LocalControl/Fire/FBBBSniperBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/LocalControl/Fire/FBBBSniperAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/LocalControl/Reload/FBBBSniperLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/LocalControl/Reload/FBBBSniperInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/RemoteMessage/Fire/FBBBSniperFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/RemoteMessage/Reload/FBBBSniperReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/RemoteMessage/Reload/FBBBSniperReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/AuthorityFact/Fire/FBBBSniperFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/AuthorityFact/Reload/FBBBSniperReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Sniper/Input/AuthorityFact/Reload/FBBBSniperReloadEndAuthorityFactPacket.h"

/** 狙击枪等待更新时消费的固定输入槽 */
struct FBBBSniperInputState final
{
    /** EndContactLocalControl固定输入槽 */
    TBBBSniperInputSlot<FBBBSniperEndContactLocalControlPacket> EndContactLocalControl;

    /** EndActionLocalControl固定输入槽 */
    TBBBSniperInputSlot<FBBBSniperEndActionLocalControlPacket> EndActionLocalControl;

    /** BeginContactLocalControl固定输入槽 */
    TBBBSniperInputSlot<FBBBSniperBeginContactLocalControlPacket> BeginContactLocalControl;

    /** BeginActionLocalControl固定输入槽 */
    TBBBSniperInputSlot<FBBBSniperBeginActionLocalControlPacket> BeginActionLocalControl;

    /** 网格显示输入槽位 */
    TBBBSniperInputSlot<FBBBSniperVisibilityAuthorityFactPacket> VisibilityAuthorityFact;
    /** 网格显示输入槽位 */
    TBBBSniperInputSlot<FBBBSniperVisibilityRemoteMessagePacket> VisibilityRemoteMessage;
    /** 网格显示输入槽位 */
    TBBBSniperInputSlot<FBBBSniperShowLocalControlPacket> ShowLocalControl;
    /** 网格显示输入槽位 */
    TBBBSniperInputSlot<FBBBSniperHideLocalControlPacket> HideLocalControl;
    /** Equip固定槽位 */
    TBBBSniperInputSlot<FBBBSniperEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBSniperInputSlot<FBBBSniperEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBSniperInputSlot<FBBBSniperPrimaryLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBSniperInputSlot<FBBBSniperSecondaryLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBSniperInputSlot<FBBBSniperBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBSniperInputSlot<FBBBSniperAllowFireLocalControlPacket> AllowFire;

    /** LoadMagazine固定槽位 */
    TBBBSniperInputSlot<FBBBSniperLoadAmmoLocalControlPacket> LoadMagazine;

    /** InterruptReload固定槽位 */
    TBBBSniperInputSlot<FBBBSniperInterruptReloadLocalControlPacket> InterruptReload;

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

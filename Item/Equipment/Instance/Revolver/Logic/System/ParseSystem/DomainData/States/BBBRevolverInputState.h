#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/AuthorityFact/Equipment/FBBBRevolverVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/RemoteMessage/Equipment/FBBBRevolverVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverHideLocalControlPacket.h"

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Logic/System/ParseSystem/DomainData/Definitions/BBBRevolverInputSlot.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/AuthorityFact/Equipment/FBBBRevolverEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Equipment/FBBBRevolverSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Fire/FBBBRevolverBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Fire/FBBBRevolverAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Reload/FBBBRevolverLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/LocalControl/Reload/FBBBRevolverInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/RemoteMessage/Fire/FBBBRevolverFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/RemoteMessage/Reload/FBBBRevolverReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/RemoteMessage/Reload/FBBBRevolverReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/AuthorityFact/Fire/FBBBRevolverFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/AuthorityFact/Reload/FBBBRevolverReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Revolver/Input/AuthorityFact/Reload/FBBBRevolverReloadEndAuthorityFactPacket.h"

/** 左轮等待更新时消费的固定输入槽 */
struct FBBBRevolverInputState final
{
    /** EndContactLocalControl固定输入槽 */
    TBBBRevolverInputSlot<FBBBRevolverEndContactLocalControlPacket> EndContactLocalControl;

    /** EndActionLocalControl固定输入槽 */
    TBBBRevolverInputSlot<FBBBRevolverEndActionLocalControlPacket> EndActionLocalControl;

    /** BeginContactLocalControl固定输入槽 */
    TBBBRevolverInputSlot<FBBBRevolverBeginContactLocalControlPacket> BeginContactLocalControl;

    /** BeginActionLocalControl固定输入槽 */
    TBBBRevolverInputSlot<FBBBRevolverBeginActionLocalControlPacket> BeginActionLocalControl;

    /** 网格显示输入槽位 */
    TBBBRevolverInputSlot<FBBBRevolverVisibilityAuthorityFactPacket> VisibilityAuthorityFact;
    /** 网格显示输入槽位 */
    TBBBRevolverInputSlot<FBBBRevolverVisibilityRemoteMessagePacket> VisibilityRemoteMessage;
    /** 网格显示输入槽位 */
    TBBBRevolverInputSlot<FBBBRevolverShowLocalControlPacket> ShowLocalControl;
    /** 网格显示输入槽位 */
    TBBBRevolverInputSlot<FBBBRevolverHideLocalControlPacket> HideLocalControl;
    /** Equip固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverPrimaryLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverSecondaryLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverAllowFireLocalControlPacket> AllowFire;

    /** LoadMagazine固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverLoadAmmoLocalControlPacket> LoadMagazine;

    /** InterruptReload固定槽位 */
    TBBBRevolverInputSlot<FBBBRevolverInterruptReloadLocalControlPacket> InterruptReload;

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

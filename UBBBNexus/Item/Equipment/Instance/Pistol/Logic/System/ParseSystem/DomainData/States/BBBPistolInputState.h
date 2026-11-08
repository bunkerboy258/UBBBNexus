#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/AuthorityFact/Equipment/FBBBPistolVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/RemoteMessage/Equipment/FBBBPistolVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolHideLocalControlPacket.h"

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Logic/System/ParseSystem/DomainData/Definitions/BBBPistolInputSlot.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/AuthorityFact/Equipment/FBBBPistolEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Equipment/FBBBPistolSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Fire/FBBBPistolBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Fire/FBBBPistolAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Reload/FBBBPistolLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/LocalControl/Reload/FBBBPistolInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/RemoteMessage/Fire/FBBBPistolFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/RemoteMessage/Reload/FBBBPistolReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/RemoteMessage/Reload/FBBBPistolReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/AuthorityFact/Fire/FBBBPistolFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/AuthorityFact/Reload/FBBBPistolReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Pistol/Input/AuthorityFact/Reload/FBBBPistolReloadEndAuthorityFactPacket.h"

/** 手枪等待更新时消费的固定输入槽 */
struct FBBBPistolInputState final
{
    /** EndContactLocalControl固定输入槽 */
    TBBBPistolInputSlot<FBBBPistolEndContactLocalControlPacket> EndContactLocalControl;

    /** EndActionLocalControl固定输入槽 */
    TBBBPistolInputSlot<FBBBPistolEndActionLocalControlPacket> EndActionLocalControl;

    /** BeginContactLocalControl固定输入槽 */
    TBBBPistolInputSlot<FBBBPistolBeginContactLocalControlPacket> BeginContactLocalControl;

    /** BeginActionLocalControl固定输入槽 */
    TBBBPistolInputSlot<FBBBPistolBeginActionLocalControlPacket> BeginActionLocalControl;

    /** 网格显示输入槽位 */
    TBBBPistolInputSlot<FBBBPistolVisibilityAuthorityFactPacket> VisibilityAuthorityFact;
    /** 网格显示输入槽位 */
    TBBBPistolInputSlot<FBBBPistolVisibilityRemoteMessagePacket> VisibilityRemoteMessage;
    /** 网格显示输入槽位 */
    TBBBPistolInputSlot<FBBBPistolShowLocalControlPacket> ShowLocalControl;
    /** 网格显示输入槽位 */
    TBBBPistolInputSlot<FBBBPistolHideLocalControlPacket> HideLocalControl;
    /** Equip固定槽位 */
    TBBBPistolInputSlot<FBBBPistolEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBPistolInputSlot<FBBBPistolEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBPistolInputSlot<FBBBPistolPrimaryLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBPistolInputSlot<FBBBPistolSecondaryLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBPistolInputSlot<FBBBPistolBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBPistolInputSlot<FBBBPistolAllowFireLocalControlPacket> AllowFire;

    /** LoadMagazine固定槽位 */
    TBBBPistolInputSlot<FBBBPistolLoadAmmoLocalControlPacket> LoadMagazine;

    /** InterruptReload固定槽位 */
    TBBBPistolInputSlot<FBBBPistolInterruptReloadLocalControlPacket> InterruptReload;

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

#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/AuthorityFact/Equipment/FBBBRifleVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/RemoteMessage/Equipment/FBBBRifleVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleHideLocalControlPacket.h"

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Logic/System/ParseSystem/DomainData/Definitions/BBBRifleInputSlot.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/AuthorityFact/Equipment/FBBBRifleEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRiflePrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Equipment/FBBBRifleSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Fire/FBBBRifleBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Fire/FBBBRifleAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Reload/FBBBRifleLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/LocalControl/Reload/FBBBRifleInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/RemoteMessage/Fire/FBBBRifleFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/RemoteMessage/Reload/FBBBRifleReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/RemoteMessage/Reload/FBBBRifleReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/AuthorityFact/Fire/FBBBRifleFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/AuthorityFact/Reload/FBBBRifleReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Rifle/Input/AuthorityFact/Reload/FBBBRifleReloadEndAuthorityFactPacket.h"

/** 步枪等待更新时消费的固定输入槽 */
struct FBBBRifleInputState final
{
    /** EndContactLocalControl固定输入槽 */
    TBBBRifleInputSlot<FBBBRifleEndContactLocalControlPacket> EndContactLocalControl;

    /** EndActionLocalControl固定输入槽 */
    TBBBRifleInputSlot<FBBBRifleEndActionLocalControlPacket> EndActionLocalControl;

    /** BeginContactLocalControl固定输入槽 */
    TBBBRifleInputSlot<FBBBRifleBeginContactLocalControlPacket> BeginContactLocalControl;

    /** BeginActionLocalControl固定输入槽 */
    TBBBRifleInputSlot<FBBBRifleBeginActionLocalControlPacket> BeginActionLocalControl;

    /** 网格显示输入槽位 */
    TBBBRifleInputSlot<FBBBRifleVisibilityAuthorityFactPacket> VisibilityAuthorityFact;
    /** 网格显示输入槽位 */
    TBBBRifleInputSlot<FBBBRifleVisibilityRemoteMessagePacket> VisibilityRemoteMessage;
    /** 网格显示输入槽位 */
    TBBBRifleInputSlot<FBBBRifleShowLocalControlPacket> ShowLocalControl;
    /** 网格显示输入槽位 */
    TBBBRifleInputSlot<FBBBRifleHideLocalControlPacket> HideLocalControl;
    /** Equip固定槽位 */
    TBBBRifleInputSlot<FBBBRifleEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBRifleInputSlot<FBBBRifleEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBRifleInputSlot<FBBBRiflePrimaryLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBRifleInputSlot<FBBBRifleSecondaryLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBRifleInputSlot<FBBBRifleBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBRifleInputSlot<FBBBRifleAllowFireLocalControlPacket> AllowFire;

    /** LoadMagazine固定槽位 */
    TBBBRifleInputSlot<FBBBRifleLoadAmmoLocalControlPacket> LoadMagazine;

    /** InterruptReload固定槽位 */
    TBBBRifleInputSlot<FBBBRifleInterruptReloadLocalControlPacket> InterruptReload;

    /** RemoteFire固定槽位 */
    TBBBRifleInputSlot<FBBBRifleFireRemoteMessagePacket> RemoteFire;

    /** RemoteReloadStart固定槽位 */
    TBBBRifleInputSlot<FBBBRifleReloadStartRemoteMessagePacket> RemoteReloadStart;

    /** RemoteReloadEnd固定槽位 */
    TBBBRifleInputSlot<FBBBRifleReloadEndRemoteMessagePacket> RemoteReloadEnd;

    /** AuthorityFire固定槽位 */
    TBBBRifleInputSlot<FBBBRifleFireAuthorityFactPacket> AuthorityFire;

    /** AuthorityReloadStart固定槽位 */
    TBBBRifleInputSlot<FBBBRifleReloadStartAuthorityFactPacket> AuthorityReloadStart;

    /** AuthorityReloadEnd固定槽位 */
    TBBBRifleInputSlot<FBBBRifleReloadEndAuthorityFactPacket> AuthorityReloadEnd;

private:
    friend struct FBBBRifleParseDomainState;
    FBBBRifleInputState() = default;
};

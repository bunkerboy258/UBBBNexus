#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/AuthorityFact/Equipment/FBBBShotgunVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/RemoteMessage/Equipment/FBBBShotgunVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunHideLocalControlPacket.h"

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Logic/System/ParseSystem/DomainData/Definitions/BBBShotgunInputSlot.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/AuthorityFact/Equipment/FBBBShotgunEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Equipment/FBBBShotgunSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Fire/FBBBShotgunBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Fire/FBBBShotgunAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Reload/FBBBShotgunLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Reload/FBBBShotgunInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/LocalControl/Reload/FBBBShotgunReloadCycleEndLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/RemoteMessage/Fire/FBBBShotgunFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/RemoteMessage/Reload/FBBBShotgunReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/RemoteMessage/Reload/FBBBShotgunReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/AuthorityFact/Fire/FBBBShotgunFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/AuthorityFact/Reload/FBBBShotgunReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Shotgun/Input/AuthorityFact/Reload/FBBBShotgunReloadEndAuthorityFactPacket.h"

/** 霰弹枪等待更新时消费的固定输入槽 */
struct FBBBShotgunInputState final
{
    /** EndContactLocalControl固定输入槽 */
    TBBBShotgunInputSlot<FBBBShotgunEndContactLocalControlPacket> EndContactLocalControl;

    /** EndActionLocalControl固定输入槽 */
    TBBBShotgunInputSlot<FBBBShotgunEndActionLocalControlPacket> EndActionLocalControl;

    /** BeginContactLocalControl固定输入槽 */
    TBBBShotgunInputSlot<FBBBShotgunBeginContactLocalControlPacket> BeginContactLocalControl;

    /** BeginActionLocalControl固定输入槽 */
    TBBBShotgunInputSlot<FBBBShotgunBeginActionLocalControlPacket> BeginActionLocalControl;

    /** 网格显示输入槽位 */
    TBBBShotgunInputSlot<FBBBShotgunVisibilityAuthorityFactPacket> VisibilityAuthorityFact;
    /** 网格显示输入槽位 */
    TBBBShotgunInputSlot<FBBBShotgunVisibilityRemoteMessagePacket> VisibilityRemoteMessage;
    /** 网格显示输入槽位 */
    TBBBShotgunInputSlot<FBBBShotgunShowLocalControlPacket> ShowLocalControl;
    /** 网格显示输入槽位 */
    TBBBShotgunInputSlot<FBBBShotgunHideLocalControlPacket> HideLocalControl;
    /** Equip固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunPrimaryLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunSecondaryLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunAllowFireLocalControlPacket> AllowFire;

    /** LoadAmmo固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunLoadAmmoLocalControlPacket> LoadAmmo;

    /** InterruptReload固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunInterruptReloadLocalControlPacket> InterruptReload;

    /** 一发装填动作自然结束的固定覆盖槽 */
    TBBBShotgunInputSlot<FBBBShotgunReloadCycleEndLocalControlPacket> ReloadCycleEnd;

    /** RemoteFire固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunFireRemoteMessagePacket> RemoteFire;

    /** RemoteReloadStart固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunReloadStartRemoteMessagePacket> RemoteReloadStart;

    /** RemoteReloadEnd固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunReloadEndRemoteMessagePacket> RemoteReloadEnd;

    /** AuthorityFire固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunFireAuthorityFactPacket> AuthorityFire;

    /** AuthorityReloadStart固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunReloadStartAuthorityFactPacket> AuthorityReloadStart;

    /** AuthorityReloadEnd固定槽位 */
    TBBBShotgunInputSlot<FBBBShotgunReloadEndAuthorityFactPacket> AuthorityReloadEnd;

private:
    friend struct FBBBShotgunParseDomainState;
    FBBBShotgunInputState() = default;
};

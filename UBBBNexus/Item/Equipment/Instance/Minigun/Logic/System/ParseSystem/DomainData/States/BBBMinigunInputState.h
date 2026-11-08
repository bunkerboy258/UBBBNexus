#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/AuthorityFact/Equipment/FBBBMinigunVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/RemoteMessage/Equipment/FBBBMinigunVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunHideLocalControlPacket.h"

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/RemoteMessage/Fire/FBBBMinigunSpinRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/AuthorityFact/Fire/FBBBMinigunSpinAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Logic/System/ParseSystem/DomainData/Definitions/BBBMinigunInputSlot.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/AuthorityFact/Equipment/FBBBMinigunEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Equipment/FBBBMinigunSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Fire/FBBBMinigunBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Fire/FBBBMinigunAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Reload/FBBBMinigunLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/LocalControl/Reload/FBBBMinigunInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/RemoteMessage/Fire/FBBBMinigunFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/RemoteMessage/Reload/FBBBMinigunReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/RemoteMessage/Reload/FBBBMinigunReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/AuthorityFact/Fire/FBBBMinigunFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/AuthorityFact/Reload/FBBBMinigunReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/Minigun/Input/AuthorityFact/Reload/FBBBMinigunReloadEndAuthorityFactPacket.h"

/** 转管机枪等待更新时消费的固定输入槽 */
struct FBBBMinigunInputState final
{
    /** EndContactLocalControl固定输入槽 */
    TBBBMinigunInputSlot<FBBBMinigunEndContactLocalControlPacket> EndContactLocalControl;

    /** EndActionLocalControl固定输入槽 */
    TBBBMinigunInputSlot<FBBBMinigunEndActionLocalControlPacket> EndActionLocalControl;

    /** BeginContactLocalControl固定输入槽 */
    TBBBMinigunInputSlot<FBBBMinigunBeginContactLocalControlPacket> BeginContactLocalControl;

    /** BeginActionLocalControl固定输入槽 */
    TBBBMinigunInputSlot<FBBBMinigunBeginActionLocalControlPacket> BeginActionLocalControl;

    /** 网格显示输入槽位 */
    TBBBMinigunInputSlot<FBBBMinigunVisibilityAuthorityFactPacket> VisibilityAuthorityFact;
    /** 网格显示输入槽位 */
    TBBBMinigunInputSlot<FBBBMinigunVisibilityRemoteMessagePacket> VisibilityRemoteMessage;
    /** 网格显示输入槽位 */
    TBBBMinigunInputSlot<FBBBMinigunShowLocalControlPacket> ShowLocalControl;
    /** 网格显示输入槽位 */
    TBBBMinigunInputSlot<FBBBMinigunHideLocalControlPacket> HideLocalControl;
    /** 收到的电机事件消息固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunSpinRemoteMessagePacket> RemoteSpin;

    /** 收到的电机权威结果固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunSpinAuthorityFactPacket> AuthoritySpin;

    /** Equip固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunPrimaryLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunSecondaryLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunAllowFireLocalControlPacket> AllowFire;

    /** LoadMagazine固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunLoadAmmoLocalControlPacket> LoadMagazine;

    /** InterruptReload固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunInterruptReloadLocalControlPacket> InterruptReload;

    /** RemoteFire固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunFireRemoteMessagePacket> RemoteFire;

    /** RemoteReloadStart固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunReloadStartRemoteMessagePacket> RemoteReloadStart;

    /** RemoteReloadEnd固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunReloadEndRemoteMessagePacket> RemoteReloadEnd;

    /** AuthorityFire固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunFireAuthorityFactPacket> AuthorityFire;

    /** AuthorityReloadStart固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunReloadStartAuthorityFactPacket> AuthorityReloadStart;

    /** AuthorityReloadEnd固定槽位 */
    TBBBMinigunInputSlot<FBBBMinigunReloadEndAuthorityFactPacket> AuthorityReloadEnd;

private:
    friend struct FBBBMinigunParseDomainState;
    FBBBMinigunInputState() = default;
};

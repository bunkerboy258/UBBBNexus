#pragma once
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGEndContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGEndActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGBeginContactLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGBeginActionLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/AuthorityFact/Equipment/FBBBLMGVisibilityAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/RemoteMessage/Equipment/FBBBLMGVisibilityRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGHideLocalControlPacket.h"

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Logic/System/ParseSystem/DomainData/Definitions/BBBLMGInputSlot.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/AuthorityFact/Equipment/FBBBLMGEquipAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGPrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Equipment/FBBBLMGSecondaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Fire/FBBBLMGBlockFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Fire/FBBBLMGAllowFireLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Reload/FBBBLMGLoadAmmoLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/LocalControl/Reload/FBBBLMGInterruptReloadLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/RemoteMessage/Fire/FBBBLMGFireRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/RemoteMessage/Reload/FBBBLMGReloadStartRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/RemoteMessage/Reload/FBBBLMGReloadEndRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/AuthorityFact/Fire/FBBBLMGFireAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/AuthorityFact/Reload/FBBBLMGReloadStartAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Equipment/Instance/LMG/Input/AuthorityFact/Reload/FBBBLMGReloadEndAuthorityFactPacket.h"

/** 轻机枪等待更新时消费的固定输入槽 */
struct FBBBLMGInputState final
{
    /** EndContactLocalControl固定输入槽 */
    TBBBLMGInputSlot<FBBBLMGEndContactLocalControlPacket> EndContactLocalControl;

    /** EndActionLocalControl固定输入槽 */
    TBBBLMGInputSlot<FBBBLMGEndActionLocalControlPacket> EndActionLocalControl;

    /** BeginContactLocalControl固定输入槽 */
    TBBBLMGInputSlot<FBBBLMGBeginContactLocalControlPacket> BeginContactLocalControl;

    /** BeginActionLocalControl固定输入槽 */
    TBBBLMGInputSlot<FBBBLMGBeginActionLocalControlPacket> BeginActionLocalControl;

    /** 网格显示输入槽位 */
    TBBBLMGInputSlot<FBBBLMGVisibilityAuthorityFactPacket> VisibilityAuthorityFact;
    /** 网格显示输入槽位 */
    TBBBLMGInputSlot<FBBBLMGVisibilityRemoteMessagePacket> VisibilityRemoteMessage;
    /** 网格显示输入槽位 */
    TBBBLMGInputSlot<FBBBLMGShowLocalControlPacket> ShowLocalControl;
    /** 网格显示输入槽位 */
    TBBBLMGInputSlot<FBBBLMGHideLocalControlPacket> HideLocalControl;
    /** Equip固定槽位 */
    TBBBLMGInputSlot<FBBBLMGEquipLocalControlPacket> Equip;

    /** AuthorityEquip固定槽位 */
    TBBBLMGInputSlot<FBBBLMGEquipAuthorityFactPacket> AuthorityEquip;

    /** Fire固定槽位 */
    TBBBLMGInputSlot<FBBBLMGPrimaryLocalControlPacket> Fire;

    /** Reload固定槽位 */
    TBBBLMGInputSlot<FBBBLMGSecondaryLocalControlPacket> Reload;

    /** BlockFire固定槽位 */
    TBBBLMGInputSlot<FBBBLMGBlockFireLocalControlPacket> BlockFire;

    /** AllowFire固定槽位 */
    TBBBLMGInputSlot<FBBBLMGAllowFireLocalControlPacket> AllowFire;

    /** LoadMagazine固定槽位 */
    TBBBLMGInputSlot<FBBBLMGLoadAmmoLocalControlPacket> LoadMagazine;

    /** InterruptReload固定槽位 */
    TBBBLMGInputSlot<FBBBLMGInterruptReloadLocalControlPacket> InterruptReload;

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

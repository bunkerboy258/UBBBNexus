#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadeShowLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadeHideLocalControlPacket.h"
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadePrimaryLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadeEquipLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadeReleaseLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/LocalControl/Use/FBBBGrenadeFinishLocalControlPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/RemoteMessage/Use/FBBBGrenadeStateRemoteMessagePacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Input/AuthorityFact/Use/FBBBGrenadeStateAuthorityFactPacket.h"
#include "BBBWork/UBBBNexus/Item/Consumable/Instance/Grenade/Logic/System/ParseSystem/DomainData/Definitions/BBBGrenadeInputSlot.h"

/** 手雷Parse领域持久状态 */
struct FBBBGrenadeInputState final
{
    TBBBGrenadeInputSlot<FBBBGrenadeShowLocalControlPacket> ShowLocalControl;

    TBBBGrenadeInputSlot<FBBBGrenadeHideLocalControlPacket> HideLocalControl;

    /** PrimaryLocalControl 固定输入槽位 */
    TBBBGrenadeInputSlot<FBBBGrenadePrimaryLocalControlPacket> PrimaryLocalControl;
    /** EquipLocalControl 固定输入槽位 */
    TBBBGrenadeInputSlot<FBBBGrenadeEquipLocalControlPacket> EquipLocalControl;
    /** ReleaseLocalControl 固定输入槽位 */
    TBBBGrenadeInputSlot<FBBBGrenadeReleaseLocalControlPacket> ReleaseLocalControl;
    /** FinishLocalControl 固定输入槽位 */
    TBBBGrenadeInputSlot<FBBBGrenadeFinishLocalControlPacket> FinishLocalControl;
    /** PermissionLocalControl 固定输入槽位 */
    /** StateRemoteMessage 固定输入槽位 */
    TBBBGrenadeInputSlot<FBBBGrenadeStateRemoteMessagePacket> StateRemoteMessage;
    /** StateAuthorityFact 固定输入槽位 */
    TBBBGrenadeInputSlot<FBBBGrenadeStateAuthorityFactPacket> StateAuthorityFact;
};
